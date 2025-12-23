@echo off
chcp 65001 > nul
echo ========================================
echo   编译语义分析器
echo ========================================
echo.

echo [1/3] 清理旧文件...
del /Q semantic_analyzer.exe 2>nul
del /Q *.o 2>nul
del /Q symbol_report.txt 2>nul
del /Q error_report.txt 2>nul

echo [2/3] 编译所有源文件...
gcc -c symbol_table.c
gcc -c type_checker.c
gcc -c semantic.c
gcc -c semantic_main.c

echo [3/3] 链接生成可执行文件...
gcc symbol_table.o type_checker.o semantic.o semantic_main.o -o semantic_analyzer.exe

if exist semantic_analyzer.exe (
    echo 编译成功！运行语义分析器...
    echo.
    semantic_analyzer.exe
) else (
    echo 编译失败！
)

echo.
pause