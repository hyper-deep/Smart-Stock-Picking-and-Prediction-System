# Deploy the built GUI (gui_qt\release\StockGUI.exe) to output\gui
# with runtime DLLs, plugins and data files. Usage: pwsh deploy_gui.ps1
$ErrorActionPreference = 'Stop'

$root = "D:\creative\Smart Stock Picking and Prediction System"
$qt   = Join-Path $root "qt\extract\6.8.0\msvc2022_64"
$src  = Join-Path $root "gui_qt\release"
$dst  = Join-Path $root "output\gui"

New-Item -ItemType Directory -Force -Path $dst | Out-Null

# 若 GUI 正在运行, 先尝试关闭它 (否则 exe 文件被占用无法覆盖)
$running = Get-Process -Name "StockGUI" -ErrorAction SilentlyContinue
if ($running) {
    Write-Host "检测到 StockGUI 正在运行, 正在关闭以更新文件..."
    $running | Stop-Process -Force
    Start-Sleep -Milliseconds 800
}

# exe + Qt runtime DLLs
Copy-Item (Join-Path $src "StockGUI.exe") $dst -Force
foreach ($dll in @('Qt6Core.dll','Qt6Gui.dll','Qt6Widgets.dll','Qt6Network.dll')) {
    Copy-Item (Join-Path $src $dll) $dst -Force -ErrorAction SilentlyContinue
}
Copy-Item (Join-Path $src "opengl32sw.dll") $dst -Force -ErrorAction SilentlyContinue
Copy-Item (Join-Path $src "d3dcompiler_47.dll") $dst -Force -ErrorAction SilentlyContinue

# plugins
Copy-Item (Join-Path $src "platforms") $dst -Recurse -Force -ErrorAction SilentlyContinue
Copy-Item (Join-Path $src "styles") $dst -Recurse -Force -ErrorAction SilentlyContinue

# data files (GUI 工作目录需要: stocks.txt / entity.txt / relation.txt / detail/)
Copy-Item (Join-Path $root "stocks.txt") $dst -Force
Copy-Item (Join-Path $root "entity.txt") $dst -Force
Copy-Item (Join-Path $root "relation.txt") $dst -Force
Copy-Item (Join-Path $root "detail") $dst -Recurse -Force

Write-Host "GUI deployed to: $dst"
Write-Host "Run: $dst\StockGUI.exe"
