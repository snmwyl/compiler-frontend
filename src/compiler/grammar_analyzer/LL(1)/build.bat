@echo off
chcp 65001 > nul
echo ========================================
echo     编译LL(1)语法分析器
echo ========================================
echo.

echo [1/5] 清理旧文件...
if exist *.exe del *.exe
if exist *.o del *.o
if exist ll1_result.txt del ll1_result.txt

echo [2/5] 编译词法分析器...
gcc -c ../../lexical_analyzer/scanner.c -o scanner.o -I../../lexical_analyzer

echo [3/5] 编译语法分析器...
gcc -c parser.c -o parser.o -I../../lexical_analyzer
gcc -c main.c -o main.o -I../../lexical_analyzer

echo [4/5] 链接生成可执行文件...
gcc scanner.o parser.o main.o -o ll1_parser.exe

if exist ll1_parser.exe (
    echo [5/5] 运行LL(1)语法分析器...
    echo.
    ll1_parser.exe
) else (
    echo 编译失败！
)

echo.
pause