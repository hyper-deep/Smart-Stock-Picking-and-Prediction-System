# One-click: rebuild GUI and deploy to output\gui.
# Usage: pwsh -ExecutionPolicy Bypass -File build_gui.bat   (double-click / VSCode task)
# This is a wrapper so it can be triggered from VSCode tasks.json easily.
pwsh -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_gui.ps1"
if errorlevel 1 (
    echo [ERROR] Build failed. Fix errors and retry.
    exit /b 1
)
pwsh -NoProfile -ExecutionPolicy Bypass -File "%~dp0deploy_gui.ps1"
if errorlevel 1 (
    echo [ERROR] Deploy failed.
    exit /b 1
)
echo.
echo [DONE] GUI rebuilt and deployed. Now run: output\gui\StockGUI.exe
