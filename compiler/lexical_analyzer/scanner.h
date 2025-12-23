#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>

// Token类型枚举
typedef enum {
    // 关键字
    TK_BEGIN = 1,   // begin
    TK_END,         // end
    TK_IF,          // if
    TK_THEN,        // then
    TK_ELSE,        // else
    TK_WHILE,       // while
    TK_DO,          // do
    TK_FOR,         // for
    TK_SWITCH,      // switch
    TK_CASE,        // case
    TK_DEFAULT,     // default
    TK_TRUE,        // true
    TK_FALSE,       // false
    
    // 标识符和常量
    TK_ID,          // 标识符
    TK_NUM,         // 数字常量
    TK_STR,         // 字符串常量
    
    // 运算符
    TK_PLUS,        // +
    TK_MINUS,       // -
    TK_MUL,         // *
    TK_DIV,         // /
    TK_ASSIGN,      // = (赋值)
    TK_EQ,          // == (等于)
    TK_NE,          // != (不等于)
    TK_LT,          // < (小于)
    TK_LE,          // <= (小于等于)
    TK_GT,          // > (大于)
    TK_GE,          // >= (大于等于)
    
    // 分隔符
    TK_LPAREN,      // (
    TK_RPAREN,      // )
    TK_LBRACE,      // {
    TK_RBRACE,      // }
    TK_SEMICOLON,   // ;
    TK_COMMA,       // ,
    TK_COLON,       // :
    
    // 特殊
    TK_EOF,         // 文件结束
    TK_ERROR        // 错误
} TokenType;

// Token结构体
typedef struct {
    TokenType type;     // Token类型
    char lexeme[256];   // Token字符串
    int line;          // 行号
    int column;        // 列号
    int int_value;     // 数值（如果是数字）
} Token;

// 关键字查找表
typedef struct {
    char* word;
    TokenType type;
} Keyword;

// 全局函数声明
void init_scanner(FILE* input);
Token get_next_token();
const char* token_type_to_string(TokenType type);
void print_token(Token token);
void close_scanner();

#endif