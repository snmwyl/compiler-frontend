#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SYMBOLS 1000
#define MAX_NAME_LEN 100

// 数据类型
typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING
} DataType;

// 符号类型
typedef enum {
    SYM_VARIABLE,
    SYM_CONSTANT,
    SYM_FUNCTION
} SymbolType;

// 符号表条目
typedef struct SymbolEntry {
    char name[MAX_NAME_LEN];
    SymbolType sym_type;
    DataType data_type;
    int scope_level;
    int line_number;
    bool initialized;
    bool used;
    struct SymbolEntry* next;
} SymbolEntry;

// 符号表
typedef struct {
    SymbolEntry* symbols;
    int scope_level;
    int symbol_count;
} SymbolTable;

// 函数声明
SymbolTable* create_symbol_table();
void destroy_symbol_table(SymbolTable* table);
SymbolEntry* insert_symbol(SymbolTable* table, const char* name, 
                          SymbolType sym_type, DataType data_type, int line);
SymbolEntry* lookup_symbol(SymbolTable* table, const char* name);
bool check_type_compatibility(DataType type1, DataType type2);
DataType get_expression_type(DataType type1, DataType type2, const char* op);
void print_symbol_table(SymbolTable* table);

#endif