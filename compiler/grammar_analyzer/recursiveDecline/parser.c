#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// 全局变量定义
Token current_token;
ParseStep steps[MAX_STEPS];
int step_count = 0;
int parse_depth = 0;
ASTNode* ast_root = NULL;

// ==================== 工具函数 ====================

// 创建AST节点
ASTNode* create_node(NodeType type, const char* value, int line, int col) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    if (value) {
        strncpy(node->value, value, 99);
        node->value[99] = '\0';
    } else {
        node->value[0] = '\0';
    }
    node->line = line;
    node->column = col;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    return node;
}

// 记录分析步骤
void add_step(const char* stack, const char* input, const char* action) {
    if (step_count >= MAX_STEPS) return;
    
    ParseStep* step = &steps[step_count];
    step->step = step_count + 1;
    step->depth = parse_depth;
    
    strncpy(step->stack, stack, 199);
    step->stack[199] = '\0';
    
    strncpy(step->input, input, 199);
    step->input[199] = '\0';
    
    strncpy(step->action, action, 99);
    step->action[99] = '\0';
    
    step_count++;
}

// 获取下一个token
static void next_token() {
    current_token = get_next_token();
}

// 匹配token
void match(TokenType expected) {
    char action[100];
    snprintf(action, sizeof(action), "匹配 %s", token_type_to_string(expected));
    
    char stack[100];
    snprintf(stack, sizeof(stack), "期望: %s", token_type_to_string(expected));
    
    char input[100];
    snprintf(input, sizeof(input), "当前: %s", current_token.lexeme);
    
    add_step(stack, input, action);
    
    if (current_token.type == expected) {
        next_token();
    } else {
        char error[200];
        snprintf(error, sizeof(error), "期望 %s, 但得到 %s", 
                token_type_to_string(expected), 
                token_type_to_string(current_token.type));
        syntax_error(error);
    }
}

// 语法错误
void syntax_error(const char* message) {
    printf("\n❌ 语法错误: %s\n", message);
    printf("   位置: 第%d行, 第%d列\n", current_token.line, current_token.column);
    printf("   当前token: %s\n", current_token.lexeme);
    exit(1);
}

// ==================== 递归下降分析函数 ====================

// factor → ID | NUM | STR | ( expression )
ASTNode* parse_factor() {
    parse_depth++;
    add_step("parse_factor", current_token.lexeme, "进入factor分析");
    
    ASTNode* node = NULL;
    
    if (current_token.type == TK_ID) {
        add_step("factor → ID", current_token.lexeme, "识别标识符");
        node = create_node(NODE_ID, current_token.lexeme, 
                          current_token.line, current_token.column);
        match(TK_ID);
    }
    else if (current_token.type == TK_NUM) {
        add_step("factor → NUM", current_token.lexeme, "识别数字");
        node = create_node(NODE_NUM, current_token.lexeme,
                          current_token.line, current_token.column);
        match(TK_NUM);
    }
    else if (current_token.type == TK_STR) {
        add_step("factor → STRING", current_token.lexeme, "识别字符串");
        node = create_node(NODE_STR, current_token.lexeme,
                          current_token.line, current_token.column);
        match(TK_STR);
    }
    else if (current_token.type == TK_LPAREN) {
        add_step("factor → ( expression )", "(", "识别括号表达式");
        match(TK_LPAREN);
        node = parse_expression();
        match(TK_RPAREN);
    }
    else {
        syntax_error("期望因子: ID, NUM, STRING 或 (expression)");
    }
    
    add_step("parse_factor", "完成", "退出factor分析");
    parse_depth--;
    return node;
}

// term → factor { (* | /) factor }
ASTNode* parse_term() {
    parse_depth++;
    add_step("parse_term", current_token.lexeme, "进入term分析");
    
    ASTNode* node = parse_factor();
    
    while (current_token.type == TK_MUL || current_token.type == TK_DIV) {
        char op[10];
        if (current_token.type == TK_MUL) {
            strcpy(op, "*");
            add_step("term → term * factor", "*", "识别乘法");
        } else {
            strcpy(op, "/");
            add_step("term → term / factor", "/", "识别除法");
        }
        
        int op_line = current_token.line;
        int op_col = current_token.column;
        match(current_token.type);
        
        ASTNode* right = parse_factor();
        ASTNode* op_node = create_node(NODE_BINARY_OP, op, op_line, op_col);
        op_node->left = node;
        op_node->right = right;
        node = op_node;
    }
    
    add_step("parse_term", "完成", "退出term分析");
    parse_depth--;
    return node;
}

// expression → term { (+ | -) term }
ASTNode* parse_expression() {
    parse_depth++;
    add_step("parse_expression", current_token.lexeme, "进入expression分析");
    
    ASTNode* node = parse_term();
    
    while (current_token.type == TK_PLUS || current_token.type == TK_MINUS) {
        char op[10];
        if (current_token.type == TK_PLUS) {
            strcpy(op, "+");
            add_step("expression → expression + term", "+", "识别加法");
        } else {
            strcpy(op, "-");
            add_step("expression → expression - term", "-", "识别减法");
        }
        
        int op_line = current_token.line;
        int op_col = current_token.column;
        match(current_token.type);
        
        ASTNode* right = parse_term();
        ASTNode* op_node = create_node(NODE_BINARY_OP, op, op_line, op_col);
        op_node->left = node;
        op_node->right = right;
        node = op_node;
    }
    
    add_step("parse_expression", "完成", "退出expression分析");
    parse_depth--;
    return node;
}

// condition → expression relop expression | expression
ASTNode* parse_condition() {
    parse_depth++;
    add_step("parse_condition", current_token.lexeme, "进入condition分析");
    
    // 解析左侧表达式
    ASTNode* left_expr = parse_expression();
    
    // 检查是否是关系运算符
    if (current_token.type == TK_GT || current_token.type == TK_LT ||
        current_token.type == TK_GE || current_token.type == TK_LE ||
        current_token.type == TK_EQ || current_token.type == TK_NE) {
        
        // 是关系表达式
        char relop[10];
        switch (current_token.type) {
            case TK_GT: strcpy(relop, ">"); break;
            case TK_LT: strcpy(relop, "<"); break;
            case TK_GE: strcpy(relop, ">="); break;
            case TK_LE: strcpy(relop, "<="); break;
            case TK_EQ: strcpy(relop, "=="); break;
            case TK_NE: strcpy(relop, "!="); break;
            default: strcpy(relop, "??"); break;
        }
        
        add_step("condition → expression relop expression", relop, "识别关系表达式");
        
        int relop_line = current_token.line;
        int relop_col = current_token.column;
        match(current_token.type);
        
        // 解析右侧表达式
        ASTNode* right_expr = parse_expression();
        
        // 创建条件节点
        ASTNode* cond_node = create_node(NODE_CONDITION, "condition", relop_line, relop_col);
        ASTNode* relop_node = create_node(NODE_RELOP, relop, relop_line, relop_col);
        
        // 构建条件树：cond_node作为根，左子节点是左表达式，右子节点是右表达式
        // 关系运算符附加到左表达式上
        cond_node->left = left_expr;
        cond_node->right = right_expr;
        left_expr->next = relop_node;  // 关系运算符作为左表达式的兄弟
        
        add_step("parse_condition", "完成", "退出condition分析");
        parse_depth--;
        return cond_node;
    } else {
        // 只是简单表达式作为条件
        add_step("condition → expression", "简单条件", "无关系运算符");
        add_step("parse_condition", "完成", "退出condition分析");
        parse_depth--;
        return left_expr;
    }
}

// assignment → ID = expression ;
ASTNode* parse_assignment() {
    parse_depth++;
    add_step("parse_assignment", current_token.lexeme, "进入assignment分析");
    
    // 保存变量名
    char var_name[100];
    strcpy(var_name, current_token.lexeme);
    int var_line = current_token.line;
    int var_col = current_token.column;
    
    add_step("assignment → ID = expression ;", var_name, "识别赋值语句");
    
    match(TK_ID);
    match(TK_ASSIGN);
    
    ASTNode* expr_node = parse_expression();
    match(TK_SEMICOLON);
    
    ASTNode* assign_node = create_node(NODE_ASSIGNMENT, var_name, var_line, var_col);
    assign_node->left = expr_node;
    
    add_step("parse_assignment", "完成", "退出assignment分析");
    parse_depth--;
    return assign_node;
}

// if_statement → if condition then statement [else statement]
ASTNode* parse_if() {
    parse_depth++;
    add_step("parse_if", current_token.lexeme, "进入if分析");
    
    int if_line = current_token.line;
    int if_col = current_token.column;
    match(TK_IF);
    
    // 解析条件表达式
    ASTNode* cond_node = parse_condition();
    match(TK_THEN);
    
    // 解析then语句
    ASTNode* then_node = parse_statement();
    
    ASTNode* if_node = create_node(NODE_IF, "if", if_line, if_col);
    if_node->left = cond_node;
    if_node->right = then_node;
    
    // 可选的else部分
    if (current_token.type == TK_ELSE) {
        add_step("if → if ... else statement", "else", "识别else分支");
        match(TK_ELSE);
        ASTNode* else_node = parse_statement();
        
        // 创建新节点存储else
        ASTNode* if_else_node = create_node(NODE_IF, "if-else", if_line, if_col);
        if_else_node->left = if_node;
        if_else_node->right = else_node;
        if_node = if_else_node;
    }
    
    add_step("parse_if", "完成", "退出if分析");
    parse_depth--;
    return if_node;
}

// while_statement → while condition do statement
ASTNode* parse_while() {
    parse_depth++;
    add_step("parse_while", current_token.lexeme, "进入while分析");
    
    int while_line = current_token.line;
    int while_col = current_token.column;
    match(TK_WHILE);
    
    // 解析条件
    ASTNode* cond_node = parse_condition();
    match(TK_DO);
    
    // 解析循环体
    ASTNode* body_node = parse_statement();
    
    ASTNode* while_node = create_node(NODE_WHILE, "while", while_line, while_col);
    while_node->left = cond_node;
    while_node->right = body_node;
    
    add_step("parse_while", "完成", "退出while分析");
    parse_depth--;
    return while_node;
}

// block → begin { statement } end
ASTNode* parse_block() {
    parse_depth++;
    add_step("parse_block", current_token.lexeme, "进入block分析");
    
    int block_line = current_token.line;
    int block_col = current_token.column;
    match(TK_BEGIN);
    
    ASTNode* block_node = create_node(NODE_BLOCK, "block", block_line, block_col);
    ASTNode* last_stmt = NULL;
    
    // 解析语句列表直到遇到end
    while (current_token.type != TK_END && current_token.type != TK_EOF) {
        ASTNode* stmt = parse_statement();
        
        // 添加到语句链表
        if (last_stmt == NULL) {
            block_node->left = stmt;
        } else {
            last_stmt->next = stmt;
        }
        last_stmt = stmt;
    }
    
    match(TK_END);
    
    add_step("parse_block", "完成", "退出block分析");
    parse_depth--;
    return block_node;
}

// statement → assignment | if | while | block
ASTNode* parse_statement() {
    parse_depth++;
    add_step("parse_statement", current_token.lexeme, "进入statement分析");
    
    ASTNode* node = NULL;
    
    if (current_token.type == TK_ID) {
        node = parse_assignment();
    }
    else if (current_token.type == TK_IF) {
        node = parse_if();
    }
    else if (current_token.type == TK_WHILE) {
        node = parse_while();
    }
    else if (current_token.type == TK_BEGIN) {
        node = parse_block();
    }
    else {
        syntax_error("期望语句开始: ID, IF, WHILE 或 BEGIN");
    }
    
    add_step("parse_statement", "完成", "退出statement分析");
    parse_depth--;
    return node;
}

// program → block
void parse_program() {
    parse_depth = 0;
    step_count = 0;
    
    add_step("parse_program", "开始", "程序分析开始");
    
    // 初始化根节点
    ast_root = create_node(NODE_PROGRAM, "program", 1, 1);
    
    // 解析程序
    ASTNode* program_block = parse_block();
    ast_root->left = program_block;
    
    if (current_token.type != TK_EOF) {
        syntax_error("期望文件结束");
    }
    
    add_step("parse_program", "完成", "程序分析完成");
}

// ==================== 辅助函数 ====================

// 递归写入AST到文件
static void write_ast_to_file(FILE* file, ASTNode* node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) fprintf(file, "  ");
    
    const char* type_str;
    switch (node->type) {
        case NODE_PROGRAM: type_str = "Program"; break;
        case NODE_BLOCK: type_str = "Block"; break;
        case NODE_STATEMENT: type_str = "Statement"; break;
        case NODE_ASSIGNMENT: type_str = "Assignment"; break;
        case NODE_IF: type_str = "If"; break;
        case NODE_WHILE: type_str = "While"; break;
        case NODE_EXPRESSION: type_str = "Expression"; break;
        case NODE_TERM: type_str = "Term"; break;
        case NODE_FACTOR: type_str = "Factor"; break;
        case NODE_ID: type_str = "ID"; break;
        case NODE_NUM: type_str = "NUM"; break;
        case NODE_STR: type_str = "STRING"; break;
        case NODE_BINARY_OP: type_str = "BinaryOp"; break;
        case NODE_CONDITION: type_str = "Condition"; break;
        case NODE_RELOP: type_str = "RelOp"; break;
        default: type_str = "Unknown"; break;
    }
    
    if (node->value[0] != '\0') {
        fprintf(file, "%s: %s", type_str, node->value);
        if (node->line > 0) fprintf(file, " (行:%d)", node->line);
        fprintf(file, "\n");
    } else {
        fprintf(file, "%s\n", type_str);
    }
    
    write_ast_to_file(file, node->left, depth + 1);
    write_ast_to_file(file, node->right, depth + 1);
    
    if (node->next) {
        write_ast_to_file(file, node->next, depth);
    }
}

// ==================== 输出函数 ====================

// 打印AST
void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    const char* type_str;
    switch (node->type) {
        case NODE_PROGRAM: type_str = "Program"; break;
        case NODE_BLOCK: type_str = "Block"; break;
        case NODE_STATEMENT: type_str = "Statement"; break;
        case NODE_ASSIGNMENT: type_str = "Assignment"; break;
        case NODE_IF: type_str = "If"; break;
        case NODE_WHILE: type_str = "While"; break;
        case NODE_EXPRESSION: type_str = "Expression"; break;
        case NODE_TERM: type_str = "Term"; break;
        case NODE_FACTOR: type_str = "Factor"; break;
        case NODE_ID: type_str = "ID"; break;
        case NODE_NUM: type_str = "NUM"; break;
        case NODE_STR: type_str = "STRING"; break;
        case NODE_BINARY_OP: type_str = "BinaryOp"; break;
        case NODE_CONDITION: type_str = "Condition"; break;
        case NODE_RELOP: type_str = "RelOp"; break;
        default: type_str = "Unknown"; break;
    }
    
    if (node->value[0] != '\0') {
        printf("%s: %s", type_str, node->value);
        if (node->line > 0) printf(" (行:%d)", node->line);
        printf("\n");
    } else {
        printf("%s\n", type_str);
    }
    
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
    
    if (node->next) {
        print_ast(node->next, depth);
    }
}

// 释放AST内存
void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);
    free(node);
}

// 显示分析过程
void display_parse_process() {
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("                递归下降语法分析过程\n");
    printf("════════════════════════════════════════════════════════════\n\n");
    
    printf("%-6s %-8s %-30s %-25s %s\n", 
           "步骤", "深度", "分析栈/状态", "当前输入", "动作");
    printf("%-6s %-8s %-30s %-25s %s\n", 
           "----", "----", "----------", "--------", "----");
    
    for (int i = 0; i < step_count; i++) {
        ParseStep* step = &steps[i];
        
        // 根据深度缩进
        char indent[20] = "";
        for (int j = 0; j < step->depth; j++) strcat(indent, "  ");
        
        char stack[150];
        snprintf(stack, sizeof(stack), "%s%s", indent, step->stack);
        
        printf("%-6d %-8d %-30s %-25s %s\n", 
               step->step,
               step->depth,
               stack,
               step->input,
               step->action);
    }
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("                    抽象语法树(AST)\n");
    printf("════════════════════════════════════════════════════════════\n\n");
    
    if (ast_root) {
        print_ast(ast_root, 0);
    }
}

// 保存结果
void save_result(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        printf("无法打开文件: %s\n", filename);
        return;
    }
    
    fprintf(f, "递归下降语法分析结果\n");
    fprintf(f, "════════════════════════════════════════════════════════════\n\n");
    
    fprintf(f, "分析过程:\n");
    fprintf(f, "%-6s %-8s %-30s %-25s %s\n", 
            "步骤", "深度", "分析栈/状态", "当前输入", "动作");
    fprintf(f, "%-6s %-8s %-30s %-25s %s\n", 
            "----", "----", "----------", "--------", "----");
    
    for (int i = 0; i < step_count; i++) {
        ParseStep* step = &steps[i];
        
        char indent[20] = "";
        for (int j = 0; j < step->depth; j++) strcat(indent, "  ");
        
        char stack[150];
        snprintf(stack, sizeof(stack), "%s%s", indent, step->stack);
        
        fprintf(f, "%-6d %-8d %-30s %-25s %s\n", 
                step->step,
                step->depth,
                stack,
                step->input,
                step->action);
    }
    
    fprintf(f, "\n抽象语法树(AST):\n");
    
    write_ast_to_file(f, ast_root, 0);
    fclose(f);
    
    printf("结果已保存到: %s\n", filename);
}