#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// 全局变量
static Token current_token;
static LL1Step steps[MAX_STEPS];
static int step_count = 0;

// 文法相关
static GrammarSymbol symbols[MAX_SYMBOLS];
static int symbol_count = 0;
static Production productions[MAX_PRODUCTIONS];
static int production_count = 0;

// 分析表
static TableEntry ll1_table[MAX_SYMBOLS][MAX_SYMBOLS];
static int first_sets[MAX_SYMBOLS][MAX_SYMBOLS];
static int follow_sets[MAX_SYMBOLS][MAX_SYMBOLS];

// 符号查找
static int find_symbol(const char* name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbols[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// 添加符号
static int add_symbol(const char* name, SymbolType type) {
    int idx = find_symbol(name);
    if (idx != -1) return idx;
    
    if (symbol_count >= MAX_SYMBOLS) {
        printf("错误：符号表已满\n");
        exit(1);
    }
    
    strcpy(symbols[symbol_count].name, name);
    symbols[symbol_count].type = type;
    symbols[symbol_count].code = symbol_count;
    symbol_count++;
    return symbol_count - 1;
}

// 添加终结符
static void add_terminal(const char* name) {
    add_symbol(name, SYM_TERMINAL);
}

// 添加非终结符
static void add_nonterminal(const char* name) {
    add_symbol(name, SYM_NONTERMINAL);
}

// 记录分析步骤
static void add_step(const char* stack, const char* input, const char* action, const char* production) {
    if (step_count >= MAX_STEPS) return;
    
    LL1Step* step = &steps[step_count];
    step->step = step_count + 1;
    
    strncpy(step->stack, stack, 199);
    strncpy(step->input, input, 199);
    strncpy(step->action, action, 99);
    strncpy(step->production, production, 99);
    
    step_count++;
}

// 初始化LL(1)分析器
void init_ll1_parser() {
    // 重置所有数据结构
    symbol_count = 0;
    production_count = 0;
    step_count = 0;
    
    // 添加特殊符号
    add_symbol("ε", SYM_EPSILON);
    add_symbol("$", SYM_END);
    add_symbol("S'", SYM_START);
}

// 加载文法
void load_grammar(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("错误：无法打开文法文件 %s\n", filename);
        exit(1);
    }
    
    char line[256];
    printf("加载文法...\n");
    
    while (fgets(line, sizeof(line), f)) {
        // 跳过空行和注释
        if (line[0] == '\n' || line[0] == '#') continue;
        
        // 去除换行符
        line[strcspn(line, "\n")] = '\0';
        
        // 分割产生式
        char left[20];
        char right[100];
        
        if (sscanf(line, "%s -> %[^\n]", left, right) == 2) {
            // 添加左部非终结符
            add_nonterminal(left);
            
            // 创建产生式
            Production* prod = &productions[production_count];
            prod->id = production_count + 1;
            strcpy(prod->left, left);
            prod->right_count = 0;
            
            // 分割右部符号
            char* token = strtok(right, " ");
            while (token && prod->right_count < 5) {
                // 判断符号类型
                if (strcmp(token, "ε") == 0) {
                    strcpy(prod->right[prod->right_count], "ε");
                } else if (isupper(token[0]) || token[0] == '_') {
                    // 非终结符（大写字母或下划线开头）
                    add_nonterminal(token);
                    strcpy(prod->right[prod->right_count], token);
                } else {
                    // 终结符
                    add_terminal(token);
                    strcpy(prod->right[prod->right_count], token);
                }
                prod->right_count++;
                token = strtok(NULL, " ");
            }
            
            printf("产生式 %d: %s -> ", prod->id, prod->left);
            for (int i = 0; i < prod->right_count; i++) {
                printf("%s ", prod->right[i]);
            }
            printf("\n");
            
            production_count++;
        }
    }
    
    fclose(f);
    printf("文法加载完成，共 %d 个产生式\n", production_count);
}

// 获取token对应的文法符号
static const char* token_to_symbol(TokenType type) {
    switch (type) {
        case TK_ID: return "id";
        case TK_NUM: return "num";
        case TK_STR: return "str";
        case TK_PLUS: return "+";
        case TK_MINUS: return "-";
        case TK_MUL: return "*";
        case TK_DIV: return "/";
        case TK_ASSIGN: return "=";
        case TK_EQ: return "==";
        case TK_NE: return "!=";
        case TK_LT: return "<";
        case TK_LE: return "<=";
        case TK_GT: return ">";
        case TK_GE: return ">=";
        case TK_LPAREN: return "(";
        case TK_RPAREN: return ")";
        case TK_LBRACE: return "{";
        case TK_RBRACE: return "}";
        case TK_SEMICOLON: return ";";
        case TK_COMMA: return ",";
        case TK_COLON: return ":";
        case TK_IF: return "if";
        case TK_THEN: return "then";
        case TK_ELSE: return "else";
        case TK_WHILE: return "while";
        case TK_DO: return "do";
        case TK_BEGIN: return "begin";
        case TK_END: return "end";
        case TK_EOF: return "$";
        default: return "error";
    }
}

// 构建FIRST集（简化版）
void build_first_sets() {
    printf("\n构建FIRST集...\n");
    
    // 初始化
    for (int i = 0; i < symbol_count; i++) {
        for (int j = 0; j < symbol_count; j++) {
            first_sets[i][j] = 0;
        }
    }
    
    // 终结符的FIRST集是它自己
    for (int i = 0; i < symbol_count; i++) {
        if (symbols[i].type == SYM_TERMINAL || symbols[i].type == SYM_EPSILON) {
            first_sets[i][i] = 1;
        }
    }
    
    // 迭代计算FIRST集
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int p = 0; p < production_count; p++) {
            Production* prod = &productions[p];
            int left_idx = find_symbol(prod->left);
            
            // 处理右部
            int all_have_epsilon = 1;
            for (int i = 0; i < prod->right_count; i++) {
                int right_idx = find_symbol(prod->right[i]);
                if (right_idx == -1) continue;
                
                // 将右部符号的FIRST集加入左部
                for (int j = 0; j < symbol_count; j++) {
                    if (first_sets[right_idx][j] && !first_sets[left_idx][j]) {
                        first_sets[left_idx][j] = 1;
                        changed = 1;
                    }
                }
                
                // 如果当前符号没有ε，则停止
                if (!first_sets[right_idx][find_symbol("ε")]) {
                    all_have_epsilon = 0;
                    break;
                }
            }
            
            // 如果所有右部符号都有ε，则左部也有ε
            if (all_have_epsilon) {
                int epsilon_idx = find_symbol("ε");
                if (!first_sets[left_idx][epsilon_idx]) {
                    first_sets[left_idx][epsilon_idx] = 1;
                    changed = 1;
                }
            }
        }
    }
    
    // 打印FIRST集
    for (int i = 0; i < symbol_count; i++) {
        if (symbols[i].type == SYM_NONTERMINAL) {
            printf("FIRST(%s) = { ", symbols[i].name);
            int first = 1;
            for (int j = 0; j < symbol_count; j++) {
                if (first_sets[i][j] && symbols[j].type == SYM_TERMINAL) {
                    if (!first) printf(", ");
                    printf("%s", symbols[j].name);
                    first = 0;
                }
            }
            if (first_sets[i][find_symbol("ε")]) {
                if (!first) printf(", ");
                printf("ε");
            }
            printf(" }\n");
        }
    }
}

// 构建FOLLOW集（简化版）
void build_follow_sets() {
    printf("\n构建FOLLOW集...\n");
    
    // 初始化
    for (int i = 0; i < symbol_count; i++) {
        for (int j = 0; j < symbol_count; j++) {
            follow_sets[i][j] = 0;
        }
    }
    
    // 开始符号的FOLLOW集包含$
    int start_idx = find_symbol("S'");
    if (start_idx != -1) {
        int end_idx = find_symbol("$");
        if (end_idx != -1) {
            follow_sets[start_idx][end_idx] = 1;
        }
    }
    
    // 迭代计算FOLLOW集
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int p = 0; p < production_count; p++) {
            Production* prod = &productions[p];
            int left_idx = find_symbol(prod->left);
            
            for (int i = 0; i < prod->right_count; i++) {
                int current_idx = find_symbol(prod->right[i]);
                if (current_idx == -1 || symbols[current_idx].type != SYM_NONTERMINAL) {
                    continue;
                }
                
                // 查看current后面的符号
                int has_epsilon = 1;
                for (int j = i + 1; j < prod->right_count; j++) {
                    int next_idx = find_symbol(prod->right[j]);
                    if (next_idx == -1) continue;
                    
                    // 将FIRST(next) - {ε}加入FOLLOW(current)
                    for (int k = 0; k < symbol_count; k++) {
                        if (first_sets[next_idx][k] && symbols[k].type == SYM_TERMINAL) {
                            if (!follow_sets[current_idx][k]) {
                                follow_sets[current_idx][k] = 1;
                                changed = 1;
                            }
                        }
                    }
                    
                    // 如果next没有ε，停止
                    if (!first_sets[next_idx][find_symbol("ε")]) {
                        has_epsilon = 0;
                        break;
                    }
                }
                
                // 如果current后面的所有符号都有ε，将FOLLOW(left)加入FOLLOW(current)
                if (has_epsilon || i == prod->right_count - 1) {
                    for (int k = 0; k < symbol_count; k++) {
                        if (follow_sets[left_idx][k] && !follow_sets[current_idx][k]) {
                            follow_sets[current_idx][k] = 1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
    
    // 打印FOLLOW集
    for (int i = 0; i < symbol_count; i++) {
        if (symbols[i].type == SYM_NONTERMINAL) {
            printf("FOLLOW(%s) = { ", symbols[i].name);
            int first = 1;
            for (int j = 0; j < symbol_count; j++) {
                if (follow_sets[i][j] && symbols[j].type == SYM_TERMINAL) {
                    if (!first) printf(", ");
                    printf("%s", symbols[j].name);
                    first = 0;
                }
            }
            if (follow_sets[i][find_symbol("$")]) {
                if (!first) printf(", ");
                printf("$");
            }
            printf(" }\n");
        }
    }
}

// 构建LL(1)分析表
void build_ll1_table() {
    printf("\n构建LL(1)分析表...\n");
    
    // 初始化分析表为-1（错误）
    for (int i = 0; i < symbol_count; i++) {
        for (int j = 0; j < symbol_count; j++) {
            ll1_table[i][j].production_id = -1;
        }
    }
    
    // 填充分析表
    for (int p = 0; p < production_count; p++) {
        Production* prod = &productions[p];
        int left_idx = find_symbol(prod->left);
        
        // 计算产生式的FIRST集
        int first_symbols[MAX_SYMBOLS] = {0};
        int all_have_epsilon = 1;
        
        for (int i = 0; i < prod->right_count; i++) {
            int right_idx = find_symbol(prod->right[i]);
            if (right_idx == -1) continue;
            
            // 如果是ε产生式
            if (strcmp(prod->right[i], "ε") == 0) {
                all_have_epsilon = 1;
                break;
            }
            
            // 添加FIRST(right) - {ε}
            for (int j = 0; j < symbol_count; j++) {
                if (first_sets[right_idx][j] && symbols[j].type == SYM_TERMINAL) {
                    first_symbols[j] = 1;
                }
            }
            
            // 如果当前符号没有ε，停止
            if (!first_sets[right_idx][find_symbol("ε")]) {
                all_have_epsilon = 0;
                break;
            }
        }
        
        // 对于FIRST中的每个终结符a，将产生式加入M[left, a]
        for (int a = 0; a < symbol_count; a++) {
            if (first_symbols[a] && symbols[a].type == SYM_TERMINAL) {
                if (ll1_table[left_idx][a].production_id != -1) {
                    printf("警告：LL(1)冲突！M[%s, %s]已有产生式%d，现在要加入产生式%d\n",
                           symbols[left_idx].name, symbols[a].name,
                           ll1_table[left_idx][a].production_id, prod->id);
                }
                ll1_table[left_idx][a].production_id = prod->id;
            }
        }
        
        // 如果ε在FIRST中，对于FOLLOW(left)中的每个终结符b，将产生式加入M[left, b]
        if (all_have_epsilon) {
            for (int b = 0; b < symbol_count; b++) {
                if (follow_sets[left_idx][b] && symbols[b].type == SYM_TERMINAL) {
                    if (ll1_table[left_idx][b].production_id != -1) {
                        printf("警告：LL(1)冲突！M[%s, %s]已有产生式%d，现在要加入产生式%d\n",
                               symbols[left_idx].name, symbols[b].name,
                               ll1_table[left_idx][b].production_id, prod->id);
                    }
                    ll1_table[left_idx][b].production_id = prod->id;
                }
            }
            // 也要加入$
            int end_idx = find_symbol("$");
            if (end_idx != -1 && follow_sets[left_idx][end_idx]) {
                ll1_table[left_idx][end_idx].production_id = prod->id;
            }
        }
    }
    
    // 设置acc
    int start_idx = find_symbol("S'");
    int end_idx = find_symbol("$");
    if (start_idx != -1 && end_idx != -1) {
        ll1_table[start_idx][end_idx].production_id = 0;  // acc
    }
    
    printf("LL(1)分析表构建完成\n");
}

// 打印分析表
void print_analysis_table() {
    printf("\nLL(1)分析表:\n");
    printf("行：非终结符，列：终结符\n\n");
    
    // 打印表头（终结符）
    printf("%-10s", "");
    for (int j = 0; j < symbol_count; j++) {
        if (symbols[j].type == SYM_TERMINAL || j == find_symbol("$")) {
            printf("%-8s", symbols[j].name);
        }
    }
    printf("\n");
    
    // 打印表格内容
    for (int i = 0; i < symbol_count; i++) {
        if (symbols[i].type == SYM_NONTERMINAL || symbols[i].type == SYM_START) {
            printf("%-10s", symbols[i].name);
            
            for (int j = 0; j < symbol_count; j++) {
                if (symbols[j].type == SYM_TERMINAL || j == find_symbol("$")) {
                    int prod_id = ll1_table[i][j].production_id;
                    if (prod_id == -1) {
                        printf("%-8s", "error");
                    } else if (prod_id == 0) {
                        printf("%-8s", "acc");
                    } else {
                        char buf[10];
                        sprintf(buf, "%d", prod_id);
                        printf("%-8s", buf);
                    }
                }
            }
            printf("\n");
        }
    }
}

// 执行LL(1)分析
void parse_input(const char* input_filename) {
    printf("\n开始LL(1)语法分析...\n");
    
    // 打开输入文件
    FILE* input = fopen(input_filename, "r");
    if (!input) {
        printf("错误：无法打开输入文件 %s\n", input_filename);
        exit(1);
    }
    
    // 初始化词法分析器
    init_scanner(input);
    current_token = get_next_token();
    
    // 分析栈
    char* stack[MAX_STACK_SIZE];
    int top = 0;
    
    // 初始化栈
    stack[top++] = "$";
    stack[top++] = "S'";  // 开始符号
    
    // 获取第一个输入符号
    const char* input_symbol = token_to_symbol(current_token.type);
    
    int step = 0;
    while (top > 0) {
        step++;
        char* X = stack[top - 1];  // 栈顶符号
        char input_str[100];
        
        // 构建栈字符串
        char stack_str[200] = "";
        for (int i = top - 1; i >= 0; i--) {
            strcat(stack_str, stack[i]);
            if (i > 0) strcat(stack_str, " ");
        }
        
        // 构建输入字符串
        char input_buf[200] = "";
        strcpy(input_buf, input_symbol);
        if (current_token.lexeme[0] != '\0') {
            strcat(input_buf, " ");
            strcat(input_buf, current_token.lexeme);
        }
        strcat(input_buf, " ...");
        
        // 如果X是终结符或$
        int x_idx = find_symbol(X);
        int a_idx = find_symbol(input_symbol);
        
        if (x_idx != -1 && (symbols[x_idx].type == SYM_TERMINAL || 
                           strcmp(X, "$") == 0)) {
            if (strcmp(X, input_symbol) == 0) {
                // 匹配
                add_step(stack_str, input_buf, "匹配", X);
                
                if (strcmp(X, "$") == 0) {
                    // 分析成功
                    add_step("$", "$", "接受", "acc");
                    break;
                }
                
                // 弹出栈顶
                top--;
                
                // 获取下一个输入符号
                if (current_token.type != TK_EOF) {
                    current_token = get_next_token();
                    input_symbol = token_to_symbol(current_token.type);
                } else {
                    input_symbol = "$";
                }
            } else {
                // 错误
                add_step(stack_str, input_buf, "错误", "不匹配");
                printf("❌ 语法错误：期望 %s，得到 %s\n", X, input_symbol);
                break;
            }
        } else {
            // X是非终结符，查表
            if (x_idx == -1 || a_idx == -1) {
                add_step(stack_str, input_buf, "错误", "符号未定义");
                printf("❌ 错误：未定义的符号\n");
                break;
            }
            
            int prod_id = ll1_table[x_idx][a_idx].production_id;
            
            if (prod_id == -1) {
                // 错误
                add_step(stack_str, input_buf, "错误", "分析表空白");
                printf("❌ 语法错误：分析表M[%s, %s]为空\n", X, input_symbol);
                break;
            } else if (prod_id == 0) {
                // 接受
                add_step(stack_str, input_buf, "接受", "acc");
                break;
            } else {
                // 使用产生式
                Production* prod = NULL;
                for (int i = 0; i < production_count; i++) {
                    if (productions[i].id == prod_id) {
                        prod = &productions[i];
                        break;
                    }
                }
                
                if (!prod) {
                    add_step(stack_str, input_buf, "错误", "产生式不存在");
                    printf("❌ 错误：产生式%d不存在\n", prod_id);
                    break;
                }
                
                // 记录动作
                char action[100];
                sprintf(action, "使用产生式 %d: %s -> ", prod->id, prod->left);
                for (int i = 0; i < prod->right_count; i++) {
                    strcat(action, prod->right[i]);
                    strcat(action, " ");
                }
                
                add_step(stack_str, input_buf, "推导", action);
                
                // 弹出栈顶
                top--;
                
                // 将产生式右部逆序压栈（ε不压栈）
                if (strcmp(prod->right[0], "ε") != 0) {
                    for (int i = prod->right_count - 1; i >= 0; i--) {
                        if (top >= MAX_STACK_SIZE) {
                            printf("❌ 错误：分析栈溢出\n");
                            return;
                        }
                        stack[top++] = prod->right[i];
                    }
                }
            }
        }
        
        if (step > 1000) {
            printf("❌ 错误：分析步骤过多，可能陷入死循环\n");
            break;
        }
    }
    
    // 关闭文件
    close_scanner();
    fclose(input);
    
    printf("LL(1)分析完成\n");
}

// 显示分析过程
void display_ll1_process() {
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("                  LL(1)语法分析过程\n");
    printf("════════════════════════════════════════════════════════════\n\n");
    
    printf("%-6s %-25s %-25s %-15s %s\n", 
           "步骤", "分析栈", "剩余输入", "动作", "产生式");
    printf("%-6s %-25s %-25s %-15s %s\n", 
           "----", "------", "--------", "----", "------");
    
    for (int i = 0; i < step_count; i++) {
        LL1Step* step = &steps[i];
        
        printf("%-6d %-25s %-25s %-15s %s\n", 
               step->step,
               step->stack,
               step->input,
               step->action,
               step->production);
    }
}

// 保存结果
void save_ll1_result(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("无法打开文件: %s\n", filename);
        return;
    }
    
    fprintf(f, "LL(1)语法分析结果\n");
    fprintf(f, "════════════════════════════════════════════════════════════\n\n");
    
    fprintf(f, "分析过程:\n");
    fprintf(f, "%-6s %-25s %-25s %-15s %s\n", 
            "步骤", "分析栈", "剩余输入", "动作", "产生式");
    fprintf(f, "%-6s %-25s %-25s %-15s %s\n", 
            "----", "------", "--------", "----", "------");
    
    for (int i = 0; i < step_count; i++) {
        LL1Step* step = &steps[i];
        
        fprintf(f, "%-6d %-25s %-25s %-15s %s\n", 
                step->step,
                step->stack,
                step->input,
                step->action,
                step->production);
    }
    
    // 保存文法信息
    fprintf(f, "\n文法信息:\n");
    fprintf(f, "总共 %d 个产生式:\n", production_count);
    for (int i = 0; i < production_count; i++) {
        fprintf(f, "%d. %s -> ", productions[i].id, productions[i].left);
        for (int j = 0; j < productions[i].right_count; j++) {
            fprintf(f, "%s ", productions[i].right[j]);
        }
        fprintf(f, "\n");
    }
    
    fclose(f);
    printf("结果已保存到: %s\n", filename);
}

// 释放资源
void free_ll1_resources() {
    // 目前没有动态分配的资源
}