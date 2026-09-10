@echo off
rem 使用 MinGW 的 gcc 编译整个工程(所有 .c 文件一起编译链接)
chcp 65001 >nul
if not exist output mkdir output

gcc -Wall -Wextra -g3 ^
    main.c ^
    stock_maintain.c ^
    stock_seek.c ^
    stock_sort.c ^
    stock_choice.c ^
    stock_prediction.c ^
    stock_statistical_analysis.c ^
    -o output\stock_system.exe

if errorlevel 1 (
    echo 编译失败
    exit /b 1
)
echo 编译成功: output\stock_system.exe
