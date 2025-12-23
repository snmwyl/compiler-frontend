# Compiler Frontend - 编译原理课程项目

一个完整的编译器前端实现，包含词法分析、语法分析（递归下降和LL(1)）、语义分析等核心模块。本项目为编译原理课程实验作业的实现。

## ✨ 项目特性

- ✅ **完整词法分析器** - 支持关键字、标识符、浮点常数、关系运算符、算术运算符等
- ✅ **递归下降语法分析** - 直观易懂，每个非终结符对应解析函数
- ✅ **LL(1)语法分析** - 基于预测分析表的高效分析方法
- ✅ **语义分析器** - 符号表管理，类型检查，四元式中间代码生成
- ✅ **完整错误处理** - 详细的错误报告和恢复机制
- ✅ **可视化分析** - 状态转换、分析过程的可视化展示
- ✅ **全面测试** - 覆盖正确用例和错误用例的测试系统

## 📁 项目结构

```
compiler-frontend/
├── compiler/                         # 编译器源代码主目录
│   ├── .vscode/                      # VS Code 配置文件
│   │   ├── c_cpp_properties.json     # C/C++ 编译配置
│   │   └── settings.json             # 编辑器设置
│   │
│   ├── lexical_analyzer/             # 实验一：词法分析模块
│   │   ├── scanner.h                 # 词法分析器头文件
│   │   ├── scanner.c                 # 词法分析器核心实现
│   │   ├── main.c                    # 词法分析主程序
│   │   ├── input.txt                 # 测试输入文件
│   │   ├── output.txt                # 词法分析输出
│   │   ├── lexer.exe                 # 可执行文件
│   │   ├── build.bat                 # 构建脚本
│   │   └── error_report.txt          # 错误报告
│   │
│   ├── grammar_analyzer/             # 实验二：语法分析模块
│   │   │
│   │   ├── recursiveDecline/         # 递归下降语法分析器
│   │   │   ├── parser.h              # 递归下降分析器头文件
│   │   │   ├── parser.c              # 递归下降分析器实现
│   │   │   ├── main.c                # 递归下降主程序
│   │   │   ├── input.txt             # 递归下降测试输入
│   │   │   ├── analysis_result.txt   # 递归下降分析结果
│   │   │   ├── recursive_parser.exe  # 可执行文件
│   │   │   ├── build.bat             # 构建脚本
│   │   │   ├── main.o                # 编译中间文件
│   │   │   ├── parser.o              # 编译中间文件
│   │   │   └── scanner.o             # 词法分析模块
│   │   │
│   │   └── LL(1)/                    # LL(1)语法分析器
│   │       ├── parser.h              # LL(1)分析器头文件
│   │       ├── parser.c              # LL(1)分析器实现
│   │       ├── main.c                # LL(1)主程序
│   │       ├── grammar.txt           # 文法定义文件
│   │       ├── input.txt             # LL(1)测试输入
│   │       ├── l1_result.txt         # LL(1)分析结果
│   │       ├── l1_parser.exe         # 可执行文件
│   │       ├── build.bat             # 构建脚本
│   │       ├── main.o                # 编译中间文件
│   │       ├── parser.o              # 编译中间文件
│   │       └── scanner.o             # 词法分析模块
│   │
│   ├── semantic_analyzer/            # 实验三：语义分析模块
│   │   ├── semantic.h                # 语义分析头文件
│   │   ├── semantic.c                # 语义分析实现
│   │   ├── symbol_table.h            # 符号表头文件
│   │   ├── symbol_table.c            # 符号表实现
│   │   ├── type_checker.h            # 类型检查器头文件
│   │   ├── type_checker.c            # 类型检查器实现
│   │   ├── semantic_main.c           # 语义分析主程序
│   │   ├── semantic_analyzer.exe     # 可执行文件
│   │   ├── build.bat                 # 构建脚本
│   │   ├── semantic.o                # 语义分析目标文件
│   │   ├── symbol_table.o            # 符号表目标文件
│   │   ├── type_checker.o            # 类型检查器目标文件
│   │   ├── semantic_main.o           # 主程序目标文件
│   │   ├── symbol_report.txt         # 符号表报告
│   │   └── error_report.txt          # 语义错误报告
│   │
│   └── test_cases/                   # 综合测试用例目录
│       ├── test1.txt                 # 测试用例1：基本表达式
│       ├── test2.txt                 # 测试用例2：控制结构
│       ├── test3.txt                 # 测试用例3：复杂类型
│       └── build_all.bat             # 完整构建测试脚本
│
├── docs/                             # 项目文档目录
│   ├── design_document.md            # 设计文档
│   ├── grammar_specification.md      # 文法规范说明
│   ├── user_manual.md                # 用户使用手册
│   └── 《编译原理》实验报告.docx     # 完整实验报告
│
├── .gitignore                        # Git忽略配置文件
├── LICENSE                           # MIT许可证文件
├── README.md                         # 项目说明文档
└── build_all.bat                     # 项目级构建脚本（可选）
```

## 🚀 快速开始

### 环境要求
- **操作系统**: Windows 10/11, Linux, 或 macOS
- **编译器**: GCC (MinGW for Windows) 或 Clang
- **开发环境**: VS Code (推荐), 或其他C语言开发环境
- **内存**: 至少2GB可用内存

### Windows 平台快速构建

#### 方法一：分模块构建
```bash
# 1. 构建词法分析器
cd compiler/lexical_analyzer
build.bat
# 运行测试
lexer.exe input.txt

# 2. 构建递归下降语法分析器（优先推荐）
cd ../grammar_analyzer/recursiveDecline
build.bat
recursive_parser.exe input.txt

# 3. 构建LL(1)语法分析器
cd ../LL(1)
build.bat
l1_parser.exe input.txt

# 4. 构建语义分析器
cd ../../semantic_analyzer
build.bat
semantic_analyzer.exe ../test_cases/test1.txt
```

#### 方法二：一键构建所有模块
```bash
cd test_cases
build_all.bat
```

### Linux/macOS 平台构建
```bash
# 如果需要，先创建Makefile
# make all        # 编译所有模块
# make clean      # 清理编译文件
# make test       # 运行测试
```

## 📖 详细使用说明

### 1. 词法分析器 (实验一)

#### 功能概述
- 识别关键字：`begin`, `end`, `if`, `then`, `else`, `while`, `do`, `for`, `switch`, `case`, `default`, `true`, `false`
- 识别标识符：字母开头的字母数字串
- 识别浮点常数：如 `3.14`, `2.71828`
- 识别运算符：关系运算符(`<`, `<=`, `==`, `!=`, `>`, `>=`)、赋值符(`=`)、算术运算符(`+`, `-`, `*`, `/`)
- 识别字符串常量：双引号包围的字符串
- 支持注释处理：单行注释(`//`)和多行注释(`/* */`)
- 提供可视化状态转换展示

#### 输入输出格式
**输入文件格式** (`input.txt`):
```c
if myid >= 1.5 then x = y
while count < 100 do count = count + 1
// 这是单行注释
/* 这是
   多行注释 */
```

**输出文件格式** (`output.txt`):
```
(IF, )
(ID, 'myid')
(GE, )
(UCON, '1.5')
(THEN, )
(ID, 'x')
(IS, )
(ID, 'y')
(WHILE, )
(ID, 'count')
(LT, )
(UCON, '100')
(DO, )
(ID, 'count')
(IS, )
(ID, 'count')
(PL, )
(UCON, '1')
```

**错误报告格式** (`error_report.txt`):
```
Error at line 3, column 10: Invalid character '@'
Error at line 5, column 1: Unterminated string constant
```

### 2. 语法分析器 (实验二)

#### 2.1 递归下降语法分析器（优先推荐）

**文法定义** (BNF格式):
```
<算术表达式> ::= <项> | <算术表达式> + <项> | <算术表达式> - <项>
<项>         ::= <因式> | <项> * <因式> | <项> / <因式>
<因式>       ::= <运算对象> | ( <算术表达式> )
<运算对象>   ::= 标识符 | 无符号常数
```

**特点**:
- 代码结构清晰，易于理解和调试
- 每个非终结符对应一个解析函数
- 支持自顶向下的语法分析
- 提供详细的错误定位

**使用方法**:
```bash
cd grammar_analyzer/recursiveDecline
recursive_parser.exe input.txt
# 分析结果保存到 analysis_result.txt
```

**示例输出**:
```
=== 递归下降语法分析开始 ===
分析表达式: x + y * (z - 3.14)
步骤1: 调用E()分析算术表达式
步骤2: 调用T()分析项
步骤3: 识别标识符 'x'
步骤4: 识别运算符 '+'
步骤5: 调用T()分析项...
分析成功！语法正确。
```

#### 2.2 LL(1)语法分析器

**文法文件** (`grammar.txt`):
```
文法规则:
E  -> T E'
E' -> + T E' | - T E' | ε
T  -> F T'
T' -> * F T' | / F T' | ε
F  -> ( E ) | i

FIRST集:
FIRST(E) = { (, i }
FIRST(E') = { +, -, ε }
FIRST(T) = { (, i }
FIRST(T') = { *, /, ε }
FIRST(F) = { (, i }

FOLLOW集:
FOLLOW(E) = { ), # }
FOLLOW(E') = { ), # }
FOLLOW(T) = { +, -, ), # }
FOLLOW(T') = { +, -, ), # }
FOLLOW(F) = { *, /, +, -, ), # }

预测分析表:
    +    -    *    /    (    )    i    #
E                        0        0        
E'  1    2                   3         3
T                        4        4         
T'  7    7    5    6         7         7      
F                        8        9

```

**使用方法**:
```bash
cd grammar_analyzer/LL(1)
l1_parser.exe input.txt
# 分析结果保存到 l1_result.txt
```

**分析过程示例**:
```
分析栈      输入串        动作
#E          i+i*i#        E→T E'
#E'T        i+i*i#        T→F T'
#E'T'F      i+i*i#        F→i
#E'T'i      i+i*i#        匹配i
#E'T'       +i*i#         T'→ε
#E'         +i*i#         E'→+ T E'
#E'T+       +i*i#         匹配+
#E'T        i*i#          T→F T'
#E'T'F      i*i#          F→i
#E'T'i      i*i#          匹配i
#E'T'       *i#           T'→* F T'
#E'T'F*     *i#           匹配*
#E'T'F      i#            F→i
#E'T'i      i#            匹配i
#E'T'       #             T'→ε
#E'         #             E'→ε
#           #             接受
```

### 3. 语义分析器 (实验三)

#### 功能概述
- **符号表管理**: 记录变量、函数信息，支持作用域嵌套
- **类型检查**: 确保操作数类型匹配，支持隐式类型转换
- **中间代码生成**: 生成四元式形式的中间代码
- **语义错误检测**: 检测未声明变量、类型不匹配、重复声明等错误

#### 数据结构
```c
// 符号表条目
typedef struct SymbolEntry {
    char name[32];           // 符号名
    int type;               // 类型: INT_TYPE, FLOAT_TYPE, STRING_TYPE
    int scope;              // 作用域层级
    int line;               // 声明行号
    struct SymbolEntry *next; // 链表指针
} SymbolEntry;

// 四元式结构
typedef struct Quadruple {
    char op[8];             // 操作符
    char arg1[32];          // 第一操作数
    char arg2[32];          // 第二操作数
    char result[32];        // 结果
    int line;              // 源代码行号
} Quadruple;
```

#### 四元式生成规则
- 算术运算: `(op, arg1, arg2, result)`
  - `(+, x, y, T1)` 表示 `T1 = x + y`
- 赋值运算: `(=, x, , y)` 表示 `y = x`
- 关系运算: `(relop, x, y, label)` 用于条件跳转

#### 使用方法
```bash
cd semantic_analyzer
semantic_analyzer.exe test_cases/test2.txt
```

**输出示例** (`symbol_report.txt`):
```
=== 符号表内容 ===
作用域0 (全局):
  变量: x      类型: int    行号: 2    值: 10
  变量: y      类型: float  行号: 3    值: 3.14
  函数: main   类型: int    行号: 1    参数: 无

作用域1 (main函数内部):
  变量: i      类型: int    行号: 5    值: 未初始化
```

**四元式输出示例**:
```
=== 生成的中间代码 ===
序号  操作符  操作数1  操作数2  结果
1     =       10             x
2     =       3.14           y
3     *       y      2.0     T1
4     =       T1            y
5     return  0
```

## 🧪 测试用例说明

### 测试文件结构
```
test_cases/
├── test1.txt     # 基本测试：简单算术表达式
├── test2.txt     # 进阶测试：控制结构(if-while)
└── test3.txt     # 综合测试：复杂表达式和函数
```

### 测试用例内容

#### test1.txt - 基本表达式测试
```c
// 简单算术表达式
x = 10 + 20 * 3.14
y = (a + b) * c / 2.0
z = -x + y
```

#### test2.txt - 控制结构测试
```c
// 包含if-while的控制流
if x > 10 then
    y = 20
else
    y = 30

while i < 100 do
    i = i + 1
    sum = sum + i
```

#### test3.txt - 综合复杂测试
```c
// 复杂表达式和类型混合
result = (base * height) / 2.0
if result >= threshold then
    status = "PASS"
else
    status = "FAIL"

// 嵌套表达式
final_score = (midterm * 0.4) + (final * 0.6) + bonus
```

### 运行完整测试
```bash
cd test_cases
build_all.bat
```
测试脚本将：
1. 编译所有模块
2. 对每个测试文件分别运行词法、语法、语义分析
3. 生成分析报告
4. 对比预期输出和实际输出
5. 汇总测试结果

## 🔧 开发与扩展指南

### 添加新的关键字
1. 修改 `lexical_analyzer/scanner.h`:
```c
#define NEW_KEYWORD 28  // 新类别码
```
2. 更新 `lexical_analyzer/scanner.c` 中的关键字表:
```c
{"newkeyword", NEW_KEYWORD},
```
3. 在 `out()` 函数中添加对应输出处理

### 扩展文法规则
#### 对于递归下降分析器:
1. 在 `grammar_analyzer/recursiveDecline/parser.c` 中添加新函数:
```c
// 新增语法规则解析函数
void parseNewRule() {
    // 解析逻辑
}
```
2. 更新函数调用关系

#### 对于LL(1)分析器:
1. 更新 `grammar_analyzer/LL(1)/grammar.txt` 文法定义
2. 重新计算FIRST集和FOLLOW集
3. 更新预测分析表

### 添加语义检查规则
1. 在 `semantic_analyzer/type_checker.c` 中添加类型检查:
```c
int checkNewTypeRule(int type1, int type2) {
    // 新的类型检查规则
    return 1; // 返回1表示通过，0表示失败
}
```
2. 更新符号表操作函数

## 🐛 故障排除

### 常见问题及解决方案

**Q1: 编译时出现 "undefined reference" 错误**
```
错误信息: undefined reference to `function_name`
解决方案: 确保所有源文件都已编译并正确链接
执行: cd 相应目录 && del *.o *.exe && build.bat
```

**Q2: 递归下降分析器报告语法错误但代码看起来正确**
```
可能原因: 文法定义与实际代码不匹配
检查: 确认文法规则是否完整，是否处理了所有可能的语法结构
调试: 添加调试输出，查看分析过程中的状态
```

**Q3: LL(1)分析器报告预测分析表冲突**
```
可能原因: 文法不是LL(1)文法
检查: 文法是否存在左递归或公共前缀
解决: 消除左递归，提取左公因子
```

**Q4: 语义分析器符号表查找失败**
```
检查: 
1. 变量是否在作用域内声明
2. 符号表是否正确构建
3. 作用域管理是否正确
调试: 输出符号表内容，检查变量查找路径
```

**Q5: 在Windows上无法运行.exe文件**
```
可能原因: 防病毒软件阻止
解决方案:
1. 以管理员身份运行命令提示符
2. 将项目目录添加到杀毒软件白名单
3. 重新编译生成可执行文件
```

### 调试技巧
1. **分步调试**:
```bash
# 单独测试词法分析
lexer.exe input.txt > lex_output.txt
# 使用词法输出作为语法分析输入
recursive_parser.exe lex_output.txt
```

2. **启用详细日志**:
在代码中定义调试宏:
```c
#define DEBUG 1
#ifdef DEBUG
    printf("调试信息: %s\n", message);
#endif
```

3. **使用小型测试用例**:
从最简单的程序开始，逐步增加复杂度:
```c
// 阶段1: 单个变量
x = 10
// 阶段2: 简单运算
x = a + b
// 阶段3: 复杂表达式
x = (a + b) * c / 2.0
```

## 📚 相关文档

详细文档位于 `docs/` 目录:

1. **设计文档** (`design_document.md`) - 系统架构、设计决策、模块设计
2. **文法规范** (`grammar_specification.md`) - 支持的文法规则、扩展方法
3. **用户手册** (`user_manual.md`) - 详细使用说明、示例、故障排除
4. **实验报告** (`《编译原理》实验报告.docx`) - 完整的课程实验报告

## 👥 贡献指南

欢迎改进本项目！贡献流程如下：

1. **Fork 仓库**: 点击GitHub页面的Fork按钮
2. **创建分支**: `git checkout -b feature/your-feature-name`
3. **提交更改**: `git commit -m 'Add some feature'`
4. **推送到分支**: `git push origin feature/your-feature-name`
5. **提交Pull Request**: 在GitHub上创建Pull Request

**代码规范**:
- 使用有意义的变量名和函数名
- 添加必要的注释
- 遵循一致的代码风格
- 为新增功能添加测试用例

## 📄 许可证

本项目基于 **MIT 许可证** 开源 - 查看 [LICENSE](LICENSE) 文件了解详情。

---

**教学提示**: 本项目是一个教学性质的编译器前端实现，旨在帮助学生理解编译原理的核心概念。项目按照实验指导书的要求，逐步完成了词法分析、语法分析和语义分析三个主要实验，适合作为编译原理课程的学习参考和实践范例。
