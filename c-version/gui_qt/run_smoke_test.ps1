# Build and run the GUI smoke test (console mode)
$ErrorActionPreference = 'Stop'

$root = "D:\creative\Smart Stock Picking and Prediction System"
$qt   = Join-Path $root "qt\extract\6.8.0\msvc2022_64"
$gui  = Join-Path $root "gui_qt"
$out  = Join-Path $gui "test_build"
New-Item -ItemType Directory -Force -Path $out | Out-Null

$vs = "C:\Program Files\Microsoft Visual Studio\18\Community"
$vcvars = Join-Path $vs "VC\Auxiliary\Build\vcvars64.bat"
$envBlock = cmd /c "`"$vcvars`" >nul 2>&1 && set" | ForEach-Object {
    if ($_ -match '^([^=]+)=(.*)$') { [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process') }
}

$inc = @(
    "-I`"$qt\include`"", "-I`"$qt\include\QtCore`"", "-I`"$qt\include\QtGui`"",
    "-I`"$qt\include\QtWidgets`"", "-I`"$root`""
)
$cflags = @('/nologo', '/std:c++17', '/permissive-', '/EHsc', '/utf-8', '/Zc:__cplusplus',
            '/DWIN32', '/D_UNICODE', '/DUNICODE', '/O2', '/MD') + $inc

# C files
$cfiles = @("stock_maintain.c","stock_seek.c","stock_statistical_analysis.c",
            "stock_choice.c","stock_prediction.c","stock_sort.c")
$objFiles = @()
foreach ($f in $cfiles) {
    $obj = Join-Path $out ($f -replace '\.c$', '.obj')
    $src = Join-Path $root $f
    cl @cflags '/c' "`"$src`"" "-Fo`"$obj`"" 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "FAILED C: $f"; exit 1 }
    $objFiles += $obj
}
# smoke_test + console_capture (C++)
foreach ($f in @("smoke_test.cpp","console_capture.cpp")) {
    $obj = Join-Path $out ($f -replace '\.cpp$', '.obj')
    $src = Join-Path $gui $f
    cl @cflags '/c' "`"$src`"" "-Fo`"$obj`"" 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { Write-Host "FAILED C++: $f"; exit 1 }
    $objFiles += $obj
}

# link console exe
$linkBat = Join-Path $out "link.bat"
$content = "@echo off`r`ncd /d `"%~dp0`"`r`nlink /nologo /SUBSYSTEM:CONSOLE /OUT:smoke_test.exe"
foreach ($o in $objFiles) { $content += " `"$o`"" }
$content += " `"$qt\lib\Qt6Core.lib`" shell32.lib user32.lib gdi32.lib advapi32.lib ole32.lib"
$content += "`r`nexit /b %errorlevel%"
[System.IO.File]::WriteAllText($linkBat, $content, [System.Text.Encoding]::ASCII)
cmd /c "call `"$vcvars`" >nul 2>&1 && call `"$linkBat`"" 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) { Write-Host "LINK FAILED"; exit 1 }

# run from project root (so stocks.txt / detail / entity.txt resolve)
Write-Host "=== running smoke test ==="
Push-Location $root
& (Join-Path $out "smoke_test.exe") 2>&1
$rc = $LASTEXITCODE
Pop-Location
Write-Host "=== smoke test exit: $rc ==="
exit $rc
