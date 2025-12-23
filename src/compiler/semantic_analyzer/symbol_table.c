#include "symbol_table.h"

// 创建符号表
SymbolTable* create_symbol_table() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->symbols = NULL;
    table->scope_level = 0;
    table->symbol_count = 0;
    return table;
}

// 销毁符号表
void destroy_symbol_table(SymbolTable* table) {
    if (!table) return;
    
    SymbolEntry* entry = table->symbols;
    while (entry) {
        SymbolEntry* next = entry->next;
        free(entry);
        entry = next;
    }
    free(table);
}

// 插入符号
SymbolEntry* insert_symbol(SymbolTable* table, const char* name, 
                          SymbolType sym_type, DataType data_type, int line) {
    // 检查是否已存在
    if (lookup_symbol(table, name)) {
        return NULL;  // 重定义
    }
    
    SymbolEntry* entry = (SymbolEntry*)malloc(sizeof(SymbolEntry));
    strncpy(entry->name, name, MAX_NAME_LEN - 1);
    entry->name[MAX_NAME_LEN - 1] = '\0';
    entry->sym_type = sym_type;
    entry->data_type = data_type;
    entry->scope_level = table->scope_level;
    entry->line_number = line;
    entry->initialized = false;
    entry->used = false;
    entry->next = table->symbols;
    table->symbols = entry;
    table->symbol_count++;
    
    return entry;
}

// 查找符号
SymbolEntry* lookup_symbol(SymbolTable* table, const char* name) {
    SymbolEntry* entry = table->symbols;
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

// 检查类型兼容性
bool check_type_compatibility(DataType type1, DataType type2) {
    if (type1 == type2) return true;
    
    // 允许的隐式转换
    if (type1 == TYPE_INT && type2 == TYPE_FLOAT) return true;
    if (type1 == TYPE_FLOAT && type2 == TYPE_INT) return true;
    
    return false;
}

// 获取表达式类型
DataType get_expression_type(DataType type1, DataType type2, const char* op) {
    // 算术运算
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
        if (type1 == TYPE_FLOAT || type2 == TYPE_FLOAT) {
            return TYPE_FLOAT;
        }
        return TYPE_INT;
    }
    
    // 关系运算
    if (strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 ||
        strcmp(op, ">") == 0 || strcmp(op, ">=") == 0 ||
        strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
        return TYPE_BOOL;
    }
    
    return TYPE_VOID;
}

// 打印符号表
void print_symbol_table(SymbolTable* table) {
    printf("=== Symbol Table ===\n");
    printf("Total symbols: %d\n\n", table->symbol_count);
    
    SymbolEntry* entry = table->symbols;
    while (entry) {
        const char* sym_type_str;
        switch (entry->sym_type) {
            case SYM_VARIABLE: sym_type_str = "Variable"; break;
            case SYM_CONSTANT: sym_type_str = "Constant"; break;
            case SYM_FUNCTION: sym_type_str = "Function"; break;
            default: sym_type_str = "Unknown"; break;
        }
        
        const char* data_type_str;
        switch (entry->data_type) {
            case TYPE_INT: data_type_str = "int"; break;
            case TYPE_FLOAT: data_type_str = "float"; break;
            case TYPE_BOOL: data_type_str = "bool"; break;
            case TYPE_STRING: data_type_str = "string"; break;
            default: data_type_str = "void"; break;
        }
        
        printf("%s (%s): %s [Line: %d, Init: %s, Used: %s]\n",
               entry->name, sym_type_str, data_type_str,
               entry->line_number,
               entry->initialized ? "Yes" : "No",
               entry->used ? "Yes" : "No");
        
        entry = entry->next;
    }
}