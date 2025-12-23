#include "scanner.h"

// 全局变量
static FILE* source_file = NULL;
static int current_line = 1;
static int current_column = 1;
static char current_char = '\0';

// 关键字表
static Keyword keywords[] = {
    {"begin", TK_BEGIN},
    {"end", TK_END},
    {"if", TK_IF},
    {"then", TK_THEN},
    {"else", TK_ELSE},
    {"while", TK_WHILE},
    {"do", TK_DO},
    {"for", TK_FOR},
    {"switch", TK_SWITCH},
    {"case", TK_CASE},
    {"default", TK_DEFAULT},
    {"true", TK_TRUE},
    {"false", TK_FALSE},
    {NULL, TK_ERROR}
};

// 初始化扫描器
void init_scanner(FILE* input) {
    source_file = input;
    current_line = 1;
    current_column = 1;
    current_char = '\0';
}

// 关闭扫描器
void close_scanner() {
    if (source_file) {
        fclose(source_file);
        source_file = NULL;
    }
}

// 获取下一个字符
static char next_char() {
    if (current_char == '\n') {
        current_line++;
        current_column = 1;
    } else {
        current_column++;
    }
    
    current_char = fgetc(source_file);
    return current_char;
}

// 回退一个字符
static void unget_char(char ch) {
    ungetc(ch, source_file);
    if (ch == '\n') {
        current_line--;
    } else {
        current_column--;
    }
    current_char = '\0';
}

// 跳过空白字符
static void skip_whitespace() {
    while (isspace(current_char)) {
        current_char = next_char();
    }
}

// 跳过注释
static void skip_comment() {
    if (current_char == '/') {
        char next = next_char();
        if (next == '/') {  // 单行注释
            while (current_char != '\n' && current_char != EOF) {
                current_char = next_char();
            }
            if (current_char == '\n') {
                current_char = next_char();
            }
        }
        else if (next == '*') {  // 多行注释
            current_char = next_char();
            while (1) {
                if (current_char == EOF) {
                    break;
                }
                if (current_char == '*') {
                    next = next_char();
                    if (next == '/') {
                        current_char = next_char();
                        break;
                    }
                    unget_char(next);
                }
                current_char = next_char();
            }
        }
        else {
            unget_char(next);  // 不是注释，回退
        }
    }
}

// 查找关键字
static TokenType lookup_keyword(char* word) {
    int i = 0;
    while (keywords[i].word != NULL) {
        if (strcmp(word, keywords[i].word) == 0) {
            return keywords[i].type;
        }
        i++;
    }
    return TK_ID;  // 不是关键字，是标识符
}

// 获取下一个Token
Token get_next_token() {
    Token token;
    
    // 跳过空白和注释
    do {
        if (current_char == '\0') {
            current_char = next_char();
        }
        skip_whitespace();
        if (current_char == '/') {
            skip_comment();
        }
    } while (isspace(current_char) || current_char == '/');
    
    // 初始化token
    token.line = current_line;
    token.column = current_column;
    token.lexeme[0] = '\0';
    
    // 检查文件结束
    if (current_char == EOF) {
        token.type = TK_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }
    
    // 识别标识符或关键字
    if (isalpha(current_char) || current_char == '_') {
        int i = 0;
        while (isalnum(current_char) || current_char == '_') {
            if (i < 255) {
                token.lexeme[i++] = current_char;
            }
            current_char = next_char();
        }
        token.lexeme[i] = '\0';
        
        // 查找是否为关键字
        token.type = lookup_keyword(token.lexeme);
        return token;
    }
    
    // 识别数字
    if (isdigit(current_char)) {
        int i = 0;
        token.int_value = 0;
        
        while (isdigit(current_char)) {
            if (i < 255) {
                token.lexeme[i++] = current_char;
                token.int_value = token.int_value * 10 + (current_char - '0');
            }
            current_char = next_char();
        }
        
        // 处理浮点数（可选）
        if (current_char == '.') {
            token.lexeme[i++] = '.';
            current_char = next_char();
            while (isdigit(current_char)) {
                if (i < 255) {
                    token.lexeme[i++] = current_char;
                }
                current_char = next_char();
            }
        }
        
        token.lexeme[i] = '\0';
        token.type = TK_NUM;
        return token;
    }
    
    // 识别字符串
    if (current_char == '"') {
        int i = 0;
        token.lexeme[i++] = '"';
        current_char = next_char();
        
        while (current_char != '"' && current_char != EOF) {
            // 处理转义字符
            if (current_char == '\\') {
                token.lexeme[i++] = '\\';
                current_char = next_char();
                switch (current_char) {
                    case 'n': token.lexeme[i++] = 'n'; break;
                    case 't': token.lexeme[i++] = 't'; break;
                    case '\\': token.lexeme[i++] = '\\'; break;
                    case '"': token.lexeme[i++] = '"'; break;
                    default: token.lexeme[i++] = current_char; break;
                }
            } else {
                token.lexeme[i++] = current_char;
            }
            
            if (i >= 254) break;  // 防止溢出
            current_char = next_char();
        }
        
        if (current_char == '"') {
            token.lexeme[i++] = '"';
            token.lexeme[i] = '\0';
            token.type = TK_STR;
            current_char = next_char();
        } else {
            token.type = TK_ERROR;
            strcpy(token.lexeme, "Unterminated string");
        }
        return token;
    }
    
    // 识别运算符和分隔符
    token.lexeme[0] = current_char;
    token.lexeme[1] = '\0';
    
    switch (current_char) {
        case '+': token.type = TK_PLUS; break;
        case '-': token.type = TK_MINUS; break;
        case '*': token.type = TK_MUL; break;
        case '/': token.type = TK_DIV; break;
        
        case '=':
            token.type = TK_ASSIGN;
            current_char = next_char();
            if (current_char == '=') {
                token.type = TK_EQ;
                token.lexeme[1] = '=';
                token.lexeme[2] = '\0';
                current_char = next_char();
            }
            break;
            
        case '<':
            token.type = TK_LT;
            current_char = next_char();
            if (current_char == '=') {
                token.type = TK_LE;
                token.lexeme[1] = '=';
                token.lexeme[2] = '\0';
                current_char = next_char();
            } else if (current_char == '>') {
                token.type = TK_NE;
                token.lexeme[1] = '>';
                token.lexeme[2] = '\0';
                current_char = next_char();
            }
            break;
            
        case '>':
            token.type = TK_GT;
            current_char = next_char();
            if (current_char == '=') {
                token.type = TK_GE;
                token.lexeme[1] = '=';
                token.lexeme[2] = '\0';
                current_char = next_char();
            }
            break;
            
        case '!':
            current_char = next_char();
            if (current_char == '=') {
                token.type = TK_NE;
                strcpy(token.lexeme, "!=");
                current_char = next_char();
            } else {
                token.type = TK_ERROR;
            }
            break;
            
        case '(': token.type = TK_LPAREN; break;
        case ')': token.type = TK_RPAREN; break;
        case '{': token.type = TK_LBRACE; break;
        case '}': token.type = TK_RBRACE; break;
        case ';': token.type = TK_SEMICOLON; break;
        case ',': token.type = TK_COMMA; break;
        case ':': token.type = TK_COLON; break;
        
        default:
            token.type = TK_ERROR;
            sprintf(token.lexeme, "Unexpected character: %c", current_char);
            break;
    }
    
    if (token.type != TK_ERROR && token.lexeme[1] == '\0') {
        current_char = next_char();
    }
    
    return token;
}

// Token类型转字符串
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TK_BEGIN: return "BEGIN";
        case TK_END: return "END";
        case TK_IF: return "IF";
        case TK_THEN: return "THEN";
        case TK_ELSE: return "ELSE";
        case TK_WHILE: return "WHILE";
        case TK_DO: return "DO";
        case TK_FOR: return "FOR";
        case TK_SWITCH: return "SWITCH";
        case TK_CASE: return "CASE";
        case TK_DEFAULT: return "DEFAULT";
        case TK_TRUE: return "TRUE";
        case TK_FALSE: return "FALSE";
        case TK_ID: return "ID";
        case TK_NUM: return "NUM";
        case TK_STR: return "STRING";
        case TK_PLUS: return "PLUS";
        case TK_MINUS: return "MINUS";
        case TK_MUL: return "MUL";
        case TK_DIV: return "DIV";
        case TK_ASSIGN: return "ASSIGN";
        case TK_EQ: return "EQ";
        case TK_NE: return "NE";
        case TK_LT: return "LT";
        case TK_LE: return "LE";
        case TK_GT: return "GT";
        case TK_GE: return "GE";
        case TK_LPAREN: return "LPAREN";
        case TK_RPAREN: return "RPAREN";
        case TK_LBRACE: return "LBRACE";
        case TK_RBRACE: return "RBRACE";
        case TK_SEMICOLON: return "SEMICOLON";
        case TK_COMMA: return "COMMA";
        case TK_COLON: return "COLON";
        case TK_EOF: return "EOF";
        case TK_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// 打印Token信息
void print_token(Token token) {
    printf("Line %3d, Col %3d: %-10s", token.line, token.column, 
           token_type_to_string(token.type));
    
    if (token.type == TK_ID || token.type == TK_NUM || token.type == TK_STR) {
        printf("  '%s'", token.lexeme);
        if (token.type == TK_NUM) {
            printf(" (value: %d)", token.int_value);
        }
    }
    printf("\n");
}