@echo off
rem =====================================================
rem  Build Qt GUI (MSVC x64 + Qt 6.8.0 msvc2022_64)
rem  Usage: build_gui.bat
rem =====================================================
setlocal

set "ROOT=%~dp0"
set "QTDIR=%ROOT%qt\extract\6.8.0\msvc2022_64"

rem 1) Load MSVC x64 environment (VS 18 Community)
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
    echo [ERROR] Cannot load MSVC environment. Check Visual Studio install path.
    exit /b 1
)

rem 2) Add Qt toolchain (qmake, moc, etc.)
set "PATH=%QTDIR%\bin;%PATH%"
set "QMAKESPEC=win32-msvc"

rem 3) Clean old build and generate Makefile
cd /d "%ROOT%gui_qt"
if exist Makefile del /q Makefile
qmake stock_gui.pro -spec win32-msvc
if errorlevel 1 (
    echo [ERROR] qmake failed to generate Makefile
    exit /b 1
)

rem 4) Compile (release)
nmake release
if errorlevel 1 (
    echo [ERROR] Build failed
    exit /b 1
)

echo.
echo [SUCCESS] Built: %ROOT%gui_qt\release\StockGUI.exe
endlocal
