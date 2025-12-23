#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "type_checker.h"

// 语义分析器
typedef struct {
    TypeCheckContext type_context;
    SymbolTable* symbol_table;
    ASTNode* ast_root;
    int warning_count;
} SemanticAnalyzer;

// 函数声明
SemanticAnalyzer* create_semantic_analyzer(ASTNode* ast);
void destroy_semantic_analyzer(SemanticAnalyzer* analyzer);
void perform_semantic_analysis(SemanticAnalyzer* analyzer);
void build_symbol_table(SemanticAnalyzer* analyzer);
void check_unused_variables(SemanticAnalyzer* analyzer);
void generate_symbol_report(SemanticAnalyzer* analyzer, const char* filename);
void generate_error_report(SemanticAnalyzer* analyzer, const char* filename);
void print_semantic_summary(SemanticAnalyzer* analyzer);

#endif