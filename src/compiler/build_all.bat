@echo off
chcp 65001 > nul
echo ========================================
echo     编译所有编译器实验
echo ========================================
echo.

echo [1/3] 编译词法分析器...
cd lexical_analyzer
gcc scanner.c main.c -o lexer.exe
if exist lexer.exe (
    echo 词法分析器编译完成！
) else (
    echo 词法分析器编译失败！
)
cd ..

echo [2/3] 编译递归下降语法分析器...
cd grammar_analyzer\recursiveDecline
call build.bat
cd ..\..

echo [3/3] 编译LL(1)语法分析器...
cd grammar_analyzer\LL1
call build.bat
cd ..\..

echo.
echo ========================================
echo     所有实验编译完成！
echo ========================================
echo.
echo 可用命令：
echo 1. lexical_analyzer\lexer.exe
echo 2. grammar_analyzer\recursiveDecline\recursive_parser.exe
echo 3. grammar_analyzer\LL1\ll1_parser.exe
echo.
pause