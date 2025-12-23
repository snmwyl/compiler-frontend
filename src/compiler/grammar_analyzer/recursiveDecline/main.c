#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

// 声明外部变量
extern Token current_token;
extern ASTNode* ast_root;

int main() {
    printf("========================================\n");
    printf("  实验二：递归下降语法分析器\n");
    printf("========================================\n\n");
    
    const char* input_file = "input.txt";
    const char* output_file = "analysis_result.txt";
    
    // 打开输入文件
    FILE* input = fopen(input_file, "r");
    if (!input) {
        printf("错误：无法打开输入文件 %s\n", input_file);
        return 1;
    }
    
    printf("输入文件: %s\n", input_file);
    printf("输出文件: %s\n\n", output_file);
    
    // 初始化词法分析器
    init_scanner(input);
    
    // 获取第一个token
    current_token = get_next_token();
    
    printf("开始语法分析...\n");
    printf("────────────────────────────────────────\n\n");
    
    // 执行语法分析
    parse_program();
    
    printf("\n✅ 语法分析完成！\n\n");
    
    // 显示分析过程
    display_parse_process();
    
    // 保存结果
    save_result(output_file);
    
    // 清理资源
    free_ast(ast_root);
    close_scanner();
    fclose(input);
    
    printf("\n========================================\n");
    
    return 0;
}