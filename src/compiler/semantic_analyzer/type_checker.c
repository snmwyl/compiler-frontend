#include "type_checker.h"

// 初始化类型检查器
void init_type_checker(TypeCheckContext* context, SymbolTable* table) {
    context->symbol_table = table;
    context->error_count = 0;
}

// 报告语义错误
void report_semantic_error(TypeCheckContext* context, ErrorType type, 
                          const char* message, int line, int col) {
    if (context->error_count >= 100) return;
    
    SemanticError* error = &context->errors[context->error_count];
    error->type = type;
    strncpy(error->message, message, 255);
    error->message[255] = '\0';
    error->line = line;
    error->column = col;
    
    context->error_count++;
}

// 打印语义错误
void print_semantic_errors(TypeCheckContext* context) {
    if (context->error_count == 0) {
        printf("No semantic errors found.\n");
        return;
    }
    
    printf("\n=== Semantic Errors (%d) ===\n", context->error_count);
    for (int i = 0; i < context->error_count; i++) {
        SemanticError* error = &context->errors[i];
        
        const char* error_type;
        switch (error->type) {
            case ERR_UNDECLARED_VAR: error_type = "Undeclared variable"; break;
            case ERR_TYPE_MISMATCH: error_type = "Type mismatch"; break;
            case ERR_UNINITIALIZED: error_type = "Uninitialized variable"; break;
            case ERR_UNUSED_VAR: error_type = "Unused variable"; break;
            default: error_type = "Unknown error"; break;
        }
        
        printf("Error [%s] at line %d, col %d: %s\n",
               error_type, error->line, error->column, error->message);
    }
}

// 类型检查表达式
DataType type_check_expression(TypeCheckContext* context, ASTNode* node) {
    if (!node) return TYPE_VOID;
    
    switch (node->type) {
        case NODE_ID: {
            const char* var_name = node->value;
            SymbolEntry* entry = lookup_symbol(context->symbol_table, var_name);
            
            if (!entry) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Undeclared variable '%s'", var_name);
                report_semantic_error(context, ERR_UNDECLARED_VAR, 
                                     msg, node->line, node->column);
                return TYPE_VOID;
            }
            
            entry->used = true;
            
            if (!entry->initialized && entry->sym_type == SYM_VARIABLE) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Uninitialized variable '%s'", var_name);
                report_semantic_error(context, ERR_UNINITIALIZED, 
                                     msg, node->line, node->column);
            }
            
            node->data_type = entry->data_type;
            return entry->data_type;
        }
        
        case NODE_NUM:
            node->data_type = TYPE_INT;
            return TYPE_INT;
            
        case NODE_STR:
            node->data_type = TYPE_STRING;
            return TYPE_STRING;
            
        case NODE_BINARY_OP: {
            DataType left_type = type_check_expression(context, node->left);
            DataType right_type = type_check_expression(context, node->right);
            
            // 对于关系运算符，返回布尔类型
            if (strcmp(node->value, ">") == 0 || strcmp(node->value, "<") == 0 ||
                strcmp(node->value, ">=") == 0 || strcmp(node->value, "<=") == 0 ||
                strcmp(node->value, "==") == 0 || strcmp(node->value, "!=") == 0) {
                node->data_type = TYPE_BOOL;
                return TYPE_BOOL;
            }
            
            // 对于算术运算符，检查类型兼容性
            if (!check_type_compatibility(left_type, right_type)) {
                char msg[256];
                snprintf(msg, sizeof(msg), 
                         "Type mismatch in binary operation '%s'", node->value);
                report_semantic_error(context, ERR_TYPE_MISMATCH, 
                                     msg, node->line, node->column);
                return TYPE_VOID;
            }
            
            DataType result_type = get_expression_type(left_type, right_type, node->value);
            node->data_type = result_type;
            return result_type;
        }
            
        case NODE_RELOP: {
            // 关系运算符：检查左右操作数都是数值类型
            DataType left_type = type_check_expression(context, node->left);
            DataType right_type = type_check_expression(context, node->right);
            
            if ((left_type != TYPE_INT && left_type != TYPE_FLOAT) ||
                (right_type != TYPE_INT && right_type != TYPE_FLOAT)) {
                char msg[256];
                snprintf(msg, sizeof(msg), 
                         "Relational operator requires numeric operands");
                report_semantic_error(context, ERR_TYPE_MISMATCH, 
                                     msg, node->line, node->column);
            }
            
            node->data_type = TYPE_BOOL;
            return TYPE_BOOL;
        }
            
        default:
            return TYPE_VOID;
    }
}

// 类型检查条件表达式
DataType type_check_condition(TypeCheckContext* context, ASTNode* node) {
    DataType cond_type = type_check_expression(context, node);
    
    // 条件必须是布尔类型
    if (cond_type != TYPE_BOOL) {
        char msg[256];
        snprintf(msg, sizeof(msg), 
                 "Condition expression must be boolean, got %s",
                 cond_type == TYPE_INT ? "int" : 
                 cond_type == TYPE_FLOAT ? "float" :
                 cond_type == TYPE_STRING ? "string" : "unknown");
        report_semantic_error(context, ERR_TYPE_MISMATCH, 
                             msg, node->line, node->column);
    }
    
    return cond_type;
}

// 类型检查赋值语句
DataType type_check_assignment(TypeCheckContext* context, ASTNode* node) {
    if (!node || node->type != NODE_ASSIGNMENT) return TYPE_VOID;
    
    const char* var_name = node->value;
    SymbolEntry* entry = lookup_symbol(context->symbol_table, var_name);
    
    if (!entry) {
        // 根据右侧表达式推断类型
        DataType rhs_type = type_check_expression(context, node->left);
        
        // 创建变量，使用右侧表达式的类型
        entry = insert_symbol(context->symbol_table, var_name, 
                             SYM_VARIABLE, rhs_type, node->line);
        if (!entry) {
            char msg[256];
            snprintf(msg, sizeof(msg), "Cannot declare variable '%s'", var_name);
            report_semantic_error(context, ERR_UNDECLARED_VAR, 
                                 msg, node->line, node->column);
            return TYPE_VOID;
        }
    }
    
    DataType rhs_type = type_check_expression(context, node->left);
    
    // 检查类型兼容性
    if (!check_type_compatibility(entry->data_type, rhs_type)) {
        char msg[256];
        snprintf(msg, sizeof(msg), 
                 "Type mismatch in assignment to '%s': expected %s, got %s", 
                 var_name,
                 entry->data_type == TYPE_INT ? "int" : 
                 entry->data_type == TYPE_FLOAT ? "float" :
                 entry->data_type == TYPE_STRING ? "string" :
                 entry->data_type == TYPE_BOOL ? "bool" : "unknown",
                 rhs_type == TYPE_INT ? "int" : 
                 rhs_type == TYPE_FLOAT ? "float" :
                 rhs_type == TYPE_STRING ? "string" :
                 rhs_type == TYPE_BOOL ? "bool" : "unknown");
        report_semantic_error(context, ERR_TYPE_MISMATCH, 
                             msg, node->line, node->column);
    }
    
    entry->initialized = true;
    entry->used = true;
    return entry->data_type;
}

// 类型检查if语句
DataType type_check_if(TypeCheckContext* context, ASTNode* node) {
    if (!node || node->type != NODE_IF) return TYPE_VOID;
    
    // 检查条件表达式
    type_check_condition(context, node->left);
    
    // 检查then分支
    type_check_statement(context, node->right);
    
    return TYPE_VOID;
}

// 类型检查while语句
DataType type_check_while(TypeCheckContext* context, ASTNode* node) {
    if (!node || node->type != NODE_WHILE) return TYPE_VOID;
    
    // 检查条件表达式
    type_check_condition(context, node->left);
    
    // 检查循环体
    type_check_statement(context, node->right);
    
    return TYPE_VOID;
}

// 类型检查语句
void type_check_statement(TypeCheckContext* context, ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_ASSIGNMENT:
            type_check_assignment(context, node);
            break;
        case NODE_IF:
            type_check_if(context, node);
            break;
        case NODE_WHILE:
            type_check_while(context, node);
            break;
        default:
            type_check_expression(context, node);
            break;
    }
}

// 类型检查程序
void type_check_program(TypeCheckContext* context, ASTNode* node) {
    if (!node) return;
    
    // 检查所有语句
    ASTNode* stmt = node;
    while (stmt) {
        type_check_statement(context, stmt);
        stmt = stmt->next;
    }
}