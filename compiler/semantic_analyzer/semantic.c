#include "semantic.h"
#include <time.h>

// 辅助函数：构建符号表
static void build_from_node(SemanticAnalyzer* analyzer, ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_ASSIGNMENT: {
            const char* var_name = node->value;
            SymbolEntry* existing = lookup_symbol(analyzer->symbol_table, var_name);
            
            if (!existing) {
                // 暂时插入为int类型，类型检查时会根据右侧表达式修正
                insert_symbol(analyzer->symbol_table, var_name, 
                             SYM_VARIABLE, TYPE_INT, node->line);
            }
            break;
        }
            
        case NODE_ID: {
            // 确保标识符在符号表中
            const char* var_name = node->value;
            SymbolEntry* existing = lookup_symbol(analyzer->symbol_table, var_name);
            if (!existing) {
                // 隐式声明
                insert_symbol(analyzer->symbol_table, var_name, 
                             SYM_VARIABLE, TYPE_INT, node->line);
            }
            break;
        }
            
        default:
            // 递归处理子节点
            build_from_node(analyzer, node->left);
            build_from_node(analyzer, node->right);
            
            // 处理兄弟节点
            ASTNode* sibling = node->next;
            while (sibling) {
                build_from_node(analyzer, sibling);
                sibling = sibling->next;
            }
            break;
    }
}

// 构建符号表
void build_symbol_table(SemanticAnalyzer* analyzer) {
    if (!analyzer || !analyzer->ast_root) return;
    
    build_from_node(analyzer, analyzer->ast_root);
}

// 创建语义分析器
SemanticAnalyzer* create_semantic_analyzer(ASTNode* ast) {
    SemanticAnalyzer* analyzer = (SemanticAnalyzer*)malloc(sizeof(SemanticAnalyzer));
    if (!analyzer) return NULL;
    
    analyzer->symbol_table = create_symbol_table();
    init_type_checker(&analyzer->type_context, analyzer->symbol_table);
    analyzer->ast_root = ast;
    analyzer->warning_count = 0;
    
    return analyzer;
}

// 销毁语义分析器
void destroy_semantic_analyzer(SemanticAnalyzer* analyzer) {
    if (!analyzer) return;
    
    destroy_symbol_table(analyzer->symbol_table);
    free(analyzer);
}

// 执行语义分析
void perform_semantic_analysis(SemanticAnalyzer* analyzer) {
    if (!analyzer || !analyzer->ast_root) return;
    
    printf("Starting semantic analysis...\n");
    printf("===============================\n\n");
    
    // 阶段1：构建符号表
    printf("Phase 1: Building symbol table...\n");
    build_symbol_table(analyzer);
    print_symbol_table(analyzer->symbol_table);
    
    // 阶段2：类型检查
    printf("\nPhase 2: Type checking...\n");
    type_check_program(&analyzer->type_context, analyzer->ast_root);
    print_semantic_errors(&analyzer->type_context);
    
    // 阶段3：其他检查
    printf("\nPhase 3: Additional semantic checks...\n");
    check_unused_variables(analyzer);
    
    // 阶段4：生成报告
    printf("\nPhase 4: Generating reports...\n");
    print_semantic_summary(analyzer);
    
    printf("\nSemantic analysis completed.\n");
}

// 检查未使用变量
void check_unused_variables(SemanticAnalyzer* analyzer) {
    printf("\nChecking for unused variables...\n");
    
    int unused_count = 0;
    SymbolEntry* entry = analyzer->symbol_table->symbols;
    
    while (entry) {
        if (entry->sym_type == SYM_VARIABLE && !entry->used) {
            printf("Warning: Unused variable '%s' declared at line %d\n",
                   entry->name, entry->line_number);
            unused_count++;
            analyzer->warning_count++;
        }
        entry = entry->next;
    }
    
    if (unused_count == 0) {
        printf("No unused variables found.\n");
    } else {
        printf("Found %d unused variable(s).\n", unused_count);
    }
}

// 生成符号表报告
void generate_symbol_report(SemanticAnalyzer* analyzer, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
    
    fprintf(file, "=== Symbol Table Report ===\n\n");
    fprintf(file, "Total symbols: %d\n\n", analyzer->symbol_table->symbol_count);
    
    SymbolEntry* entry = analyzer->symbol_table->symbols;
    while (entry) {
        const char* data_type_str;
        switch (entry->data_type) {
            case TYPE_INT: data_type_str = "int"; break;
            case TYPE_FLOAT: data_type_str = "float"; break;
            case TYPE_BOOL: data_type_str = "bool"; break;
            case TYPE_STRING: data_type_str = "string"; break;
            default: data_type_str = "void"; break;
        }
        
        fprintf(file, "%s: %s [Line: %d, Init: %s, Used: %s]\n",
               entry->name, data_type_str,
               entry->line_number,
               entry->initialized ? "Yes" : "No",
               entry->used ? "Yes" : "No");
        
        entry = entry->next;
    }
    
    fclose(file);
    printf("Symbol report saved to %s\n", filename);
}

// 生成错误报告
void generate_error_report(SemanticAnalyzer* analyzer, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
    
    TypeCheckContext* context = &analyzer->type_context;
    
    fprintf(file, "=== Semantic Error Report ===\n\n");
    fprintf(file, "Total errors: %d\n\n", context->error_count);
    
    if (context->error_count == 0) {
        fprintf(file, "No semantic errors found.\n");
    } else {
        for (int i = 0; i < context->error_count; i++) {
            SemanticError* error = &context->errors[i];
            fprintf(file, "Error at line %d, col %d: %s\n",
                   error->line, error->column, error->message);
        }
    }
    
    fclose(file);
    printf("Error report saved to %s\n", filename);
}

// 打印语义分析摘要
void print_semantic_summary(SemanticAnalyzer* analyzer) {
    printf("\n=== Semantic Analysis Summary ===\n");
    printf("Total symbols: %d\n", analyzer->symbol_table->symbol_count);
    printf("Type errors: %d\n", analyzer->type_context.error_count);
    printf("Warnings: %d\n", analyzer->warning_count);
    
    if (analyzer->type_context.error_count == 0) {
        printf("\n✅ Semantic analysis passed successfully!\n");
    } else {
        printf("\n❌ Semantic analysis failed with %d error(s).\n", 
               analyzer->type_context.error_count);
    }
}