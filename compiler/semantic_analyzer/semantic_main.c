#include "semantic.h"
#include <time.h>

// 创建节点函数
ASTNode* create_node(NodeType type, const char* value, int line) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    strncpy(node->value, value, 99);
    node->value[99] = '\0';
    node->line = line;
    node->column = 1;
    node->data_type = TYPE_VOID;
    node->left = node->right = node->next = NULL;
    return node;
}

// 创建正确的测试AST
ASTNode* create_test_ast() {
    // 创建测试程序：
    // 1. x = 10 + 20;           // x 是 int
    // 2. y = x * 3;             // y 是 int
    // 3. if (x > 15) then       // 条件表达式
    // 4.   z = "Greater"        // z 应该是 string 类型
    
    // 语句1: x = 10 + 20;
    ASTNode* assign1 = create_node(NODE_ASSIGNMENT, "x", 1);
    ASTNode* add = create_node(NODE_BINARY_OP, "+", 1);
    add->left = create_node(NODE_NUM, "10", 1);
    add->right = create_node(NODE_NUM, "20", 1);
    assign1->left = add;
    
    // 语句2: y = x * 3;
    ASTNode* assign2 = create_node(NODE_ASSIGNMENT, "y", 2);
    ASTNode* mul = create_node(NODE_BINARY_OP, "*", 2);
    mul->left = create_node(NODE_ID, "x", 2);
    mul->right = create_node(NODE_NUM, "3", 2);
    assign2->left = mul;
    
    // 语句3: if x > 15 then z = "Greater"
    ASTNode* if_stmt = create_node(NODE_IF, "if", 3);
    
    // 创建条件表达式: x > 15
    ASTNode* gt = create_node(NODE_RELOP, ">", 3);
    gt->left = create_node(NODE_ID, "x", 3);
    gt->right = create_node(NODE_NUM, "15", 3);
    if_stmt->left = gt;  // 条件作为左孩子
    
    // then分支: z = "Greater"
    ASTNode* then_assign = create_node(NODE_ASSIGNMENT, "z", 4);
    then_assign->left = create_node(NODE_STR, "Greater", 4);  // 注意：去掉引号
    if_stmt->right = then_assign;  // then语句作为右孩子
    
    // 连接所有语句
    assign1->next = assign2;
    assign2->next = if_stmt;
    
    return assign1;
}

int main() {
    printf("========================================\n");
    printf("  实验三：语义分析程序设计与实现\n");
    printf("========================================\n\n");
    
    clock_t start_time = clock();
    
    printf("[1/4] Creating test AST...\n");
    ASTNode* ast = create_test_ast();
    
    printf("[2/4] Creating semantic analyzer...\n");
    SemanticAnalyzer* analyzer = create_semantic_analyzer(ast);
    if (!analyzer) {
        printf("Error: Failed to create semantic analyzer\n");
        return 1;
    }
    
    printf("[3/4] Performing semantic analysis...\n");
    perform_semantic_analysis(analyzer);
    
    printf("[4/4] Generating reports...\n");
    generate_symbol_report(analyzer, "symbol_report.txt");
    generate_error_report(analyzer, "error_report.txt");
    
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("\n========================================\n");
    printf("  语义分析完成！\n");
    printf("  耗时: %.3f 秒\n", elapsed_time);
    printf("========================================\n");
    
    // 清理AST内存
    void free_ast(ASTNode* node) {
        if (!node) return;
        free_ast(node->left);
        free_ast(node->right);
        free_ast(node->next);
        free(node);
    }
    free_ast(ast);
    
    destroy_semantic_analyzer(analyzer);
    
    printf("\nPress Enter to exit...");
    getchar();
    
    return 0;
}