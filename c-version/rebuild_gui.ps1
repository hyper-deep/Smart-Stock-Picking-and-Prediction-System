# One-click: rebuild GUI and deploy to output\gui.
# Usage: pwsh -ExecutionPolicy Bypass -File rebuild_gui.ps1
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host "=== [1/2] 编译 GUI (build_gui.ps1) ==="
& (Join-Path $root "build_gui.ps1")
if ($LASTEXITCODE -ne 0) { Write-Host "[ERROR] 编译失败, 请先修复错误"; exit 1 }

Write-Host "=== [2/2] 部署到 output\gui (deploy_gui.ps1) ==="
& (Join-Path $root "deploy_gui.ps1")
if ($LASTEXITCODE -ne 0) { Write-Host "[ERROR] 部署失败"; exit 1 }

Write-Host ""
Write-Host "[DONE] GUI 已重新构建并部署。现在请运行: output\gui\StockGUI.exe"
Write-Host "       (在 VSCode 中可用任务面板/快捷键重新运行 GUI)"
