#ifndef RECURSIVE_PARSER_H
#define RECURSIVE_PARSER_H

#include "../../lexical_analyzer/scanner.h"

#define MAX_STEPS 1000

// 语法树节点类型
typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_STATEMENT,
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_WHILE,
    NODE_EXPRESSION,
    NODE_TERM,
    NODE_FACTOR,
    NODE_ID,
    NODE_NUM,
    NODE_STR,
    NODE_BINARY_OP,
    NODE_CONDITION,
    NODE_RELOP
} NodeType;

// 语法树节点
typedef struct ASTNode {
    NodeType type;
    char value[100];
    int line;
    int column;
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* next;
} ASTNode;

// 分析步骤记录
typedef struct {
    int step;
    char stack[200];
    char input[200];
    char action[100];
    int depth;
} ParseStep;

// 全局变量声明
extern Token current_token;
extern ASTNode* ast_root;
extern ParseStep steps[MAX_STEPS];
extern int step_count;
extern int parse_depth;

// 语法分析函数
void parse_program();
ASTNode* parse_block();
ASTNode* parse_statement();
ASTNode* parse_assignment();
ASTNode* parse_if();
ASTNode* parse_while();
ASTNode* parse_expression();
ASTNode* parse_term();
ASTNode* parse_factor();
ASTNode* parse_condition();

// 工具函数
ASTNode* create_node(NodeType type, const char* value, int line, int col);
void add_step(const char* stack, const char* input, const char* action);
void display_parse_process();
void save_result(const char* filename);
void print_ast(ASTNode* node, int depth);
void free_ast(ASTNode* node);

// 语法检查函数
void match(TokenType expected);
void syntax_error(const char* message);

#endif