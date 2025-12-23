#ifndef LL1_PARSER_H
#define LL1_PARSER_H

#include "../../lexical_analyzer/scanner.h"

#define MAX_STACK_SIZE 100
#define MAX_STEPS 500
#define MAX_SYMBOLS 50
#define MAX_PRODUCTIONS 100

// 文法符号类型
typedef enum {
    SYM_TERMINAL,      // 终结符
    SYM_NONTERMINAL,   // 非终结符
    SYM_EPSILON,       // ε（空串）
    SYM_END,           // 结束符$
    SYM_START          // 开始符号
} SymbolType;

// 文法符号
typedef struct {
    char name[20];     // 符号名称
    SymbolType type;   // 符号类型
    int code;          // 内部编码
} GrammarSymbol;

// 产生式
typedef struct {
    int id;                    // 产生式编号
    char left[20];            // 左部
    char right[5][20];        // 右部符号（最多5个）
    int right_count;          // 右部符号数量
} Production;

// 分析步骤记录
typedef struct {
    int step;
    char stack[200];          // 分析栈内容
    char input[200];          // 剩余输入
    char action[100];         // 动作
    char production[100];     // 使用的产生式
} LL1Step;

// 分析表项
typedef struct {
    int production_id;        // 产生式编号，-1表示错误，0表示acc
} TableEntry;

// 全局函数声明
void init_ll1_parser();
void load_grammar(const char* filename);
void build_first_sets();
void build_follow_sets();
void build_ll1_table();
void parse_input(const char* input_filename);
void display_ll1_process();
void save_ll1_result(const char* filename);
void print_analysis_table();
void free_ll1_resources();

#endif