# Build Qt GUI directly with cl.exe (bypasses qmake's compiler probing
# which the sandbox blocks). Usage: pwsh build_gui.ps1
$ErrorActionPreference = 'Stop'

$root = "D:\creative\Smart Stock Picking and Prediction System"
$qt   = Join-Path $root "qt\extract\6.8.0\msvc2022_64"
$gui  = Join-Path $root "gui_qt"
$out  = Join-Path $gui "release"
New-Item -ItemType Directory -Force -Path $out | Out-Null

# ---- 1) MSVC environment via vcvars ----
$vs = "C:\Program Files\Microsoft Visual Studio\18\Community"
$vcvars = Join-Path $vs "VC\Auxiliary\Build\vcvars64.bat"
$envBlock = cmd /c "`"$vcvars`" >nul 2>&1 && set" | ForEach-Object {
    if ($_ -match '^([^=]+)=(.*)$') { [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process') }
}
Write-Host "MSVC env loaded."
Write-Host "cl version:"
$clVer = & cmd /c "cl 2>&1" | Select-String "Version" | Select-Object -First 1
Write-Host $clVer.Line

# ---- 2) moc ----
$moc = Join-Path $qt "bin\moc.exe"
& $moc (Join-Path $gui "mainwindow.h") -o (Join-Path $gui "moc_mainwindow.cpp")
if ($LASTEXITCODE -ne 0) { throw "moc failed" }
Write-Host "moc done"

# ---- 3) compile flags ----
$inc = @(
    "-I`"$qt\include`"",
    "-I`"$qt\include\QtCore`"",
    "-I`"$qt\include\QtGui`"",
    "-I`"$qt\include\QtWidgets`"",
    "-I`"$root`""   # stock headers
)
$cflags = @('/nologo', '/std:c++17', '/permissive-', '/EHsc', '/utf-8', '/Zc:__cplusplus',
            '/DWIN32', '/D_UNICODE', '/DUNICODE', '/O2', '/MD') + $inc

# C files (compiled as C)
$cfiles = @(
    "stock_maintain.c",
    "stock_seek.c",
    "stock_statistical_analysis.c",
    "stock_choice.c",
    "stock_prediction.c",
    "stock_sort.c"
)
# C++ files
$cppfiles = @(
    "main.cpp",
    "mainwindow.cpp",
    "console_capture.cpp",
    "moc_mainwindow.cpp"
)

$objFiles = @()
foreach ($f in $cfiles) {
    $obj = Join-Path $out ($f -replace '\.c$', '.obj')
    $src = Join-Path $root $f
    cl @cflags '/c' "`"$src`"" "-Fo`"$obj`"" 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "FAILED: $f"; exit 1 }
    $objFiles += $obj
    Write-Host "C ok: $f"
}
foreach ($f in $cppfiles) {
    $obj = Join-Path $out ($f -replace '\.cpp$', '.obj')
    $src = Join-Path $gui $f
    cl @cflags '/c' "`"$src`"" "-Fo`"$obj`"" 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "FAILED: $f"; exit 1 }
    $objFiles += $obj
    Write-Host "C++ ok: $f"
}

# ---- 4) link ----
$libs = @(
    "`"$qt\lib\Qt6Core.lib`"",
    "`"$qt\lib\Qt6Gui.lib`"",
    "`"$qt\lib\Qt6Widgets.lib`"",
    'shell32.lib',
    'user32.lib',
    'gdi32.lib',
    'advapi32.lib',
    'ole32.lib'
)
$exe = Join-Path $out "StockGUI.exe"
# ---- 4) link (via batch in release dir, avoids path-space issues) ----
$linkBat = Join-Path $out "link2.bat"
$objs = @('stock_maintain.obj','stock_seek.obj','stock_statistical_analysis.obj',
          'stock_choice.obj','stock_prediction.obj','stock_sort.obj',
          'main.obj','mainwindow.obj','console_capture.obj','moc_mainwindow.obj')
$content = "@echo off`r`ncd /d `"%~dp0`"`r`nlink /nologo /SUBSYSTEM:WINDOWS /STACK:8388608 /OUT:StockGUI.exe"
foreach ($o in $objs) { $content += " `"$o`"" }
$content += " `"$qt\lib\Qt6Core.lib`" `"$qt\lib\Qt6Gui.lib`" `"$qt\lib\Qt6Widgets.lib`" `"$qt\lib\Qt6EntryPoint.lib`" shell32.lib user32.lib gdi32.lib advapi32.lib ole32.lib"
$content += "`r`nexit /b %errorlevel%"
[System.IO.File]::WriteAllText($linkBat, $content, [System.Text.Encoding]::ASCII)

$vcvars = "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cmd /c "call `"$vcvars`" >nul 2>&1 && call `"$linkBat`"" 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) { Write-Host "LINK FAILED"; exit 1 }
Write-Host "LINK ok -> $exe"

# ---- 5) copy runtime DLLs & plugins ----
$bin = Join-Path $qt "bin"
foreach ($dll in @('Qt6Core.dll','Qt6Gui.dll','Qt6Widgets.dll','Qt6Network.dll')) {
    Copy-Item (Join-Path $bin $dll) $out -Force
}
# platform plugin dir
$platDir = Join-Path $out "platforms"
New-Item -ItemType Directory -Force -Path $platDir | Out-Null
Copy-Item (Join-Path $qt "plugins\platforms\qwindows.dll") $platDir -Force
Copy-Item (Join-Path $qt "plugins\platforms\qminimal.dll") $platDir -Force
# styles plugin
$stylesDir = Join-Path $out "styles"
New-Item -ItemType Directory -Force -Path $stylesDir | Out-Null
Copy-Item (Join-Path $qt "plugins\styles\*.dll") $stylesDir -Force
# opengl software
Copy-Item (Join-Path $qt "bin\opengl32sw.dll") $out -Force -ErrorAction SilentlyContinue
Copy-Item (Join-Path $qt "bin\d3dcompiler_47.dll") $out -Force -ErrorAction SilentlyContinue

Write-Host "BUILD COMPLETE: $exe"
