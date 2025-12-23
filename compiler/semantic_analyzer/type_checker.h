#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "symbol_table.h"

// AST节点类型
typedef enum {
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_WHILE,
    NODE_EXPRESSION,
    NODE_ID,
    NODE_NUM,
    NODE_STR,
    NODE_BINARY_OP,
    NODE_RELOP,           // 关系运算符节点
    NODE_CONDITION        // 条件节点
} NodeType;

// AST节点
typedef struct ASTNode {
    NodeType type;
    char value[100];
    DataType data_type;
    int line;
    int column;
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* next;
} ASTNode;

// 语义错误类型
typedef enum {
    ERR_UNDECLARED_VAR,
    ERR_TYPE_MISMATCH,
    ERR_UNINITIALIZED,
    ERR_UNUSED_VAR
} ErrorType;

// 语义错误
typedef struct {
    ErrorType type;
    char message[256];
    int line;
    int column;
} SemanticError;

// 类型检查上下文
typedef struct {
    SymbolTable* symbol_table;
    SemanticError errors[100];
    int error_count;
} TypeCheckContext;

// 函数声明
void init_type_checker(TypeCheckContext* context, SymbolTable* table);
void type_check_program(TypeCheckContext* context, ASTNode* node);
void type_check_statement(TypeCheckContext* context, ASTNode* node);
DataType type_check_expression(TypeCheckContext* context, ASTNode* node);
DataType type_check_assignment(TypeCheckContext* context, ASTNode* node);
DataType type_check_if(TypeCheckContext* context, ASTNode* node);
DataType type_check_while(TypeCheckContext* context, ASTNode* node);
DataType type_check_condition(TypeCheckContext* context, ASTNode* node);
void report_semantic_error(TypeCheckContext* context, ErrorType type, 
                          const char* message, int line, int col);
void print_semantic_errors(TypeCheckContext* context);

#endif