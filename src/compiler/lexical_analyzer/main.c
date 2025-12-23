#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("      实验一：词法分析器\n");
    printf("========================================\n\n");
    
    const char* input_file = "input.txt";
    const char* output_file = "output.txt";
    
    // 打开输入文件
    FILE* input = fopen(input_file, "r");
    if (!input) {
        printf("错误：无法打开输入文件 %s\n", input_file);
        return 1;
    }
    
    // 打开输出文件
    FILE* output = fopen(output_file, "w");
    if (!output) {
        printf("错误：无法打开输出文件 %s\n", output_file);
        fclose(input);
        return 1;
    }
    
    printf("输入文件: %s\n", input_file);
    printf("输出文件: %s\n\n", output_file);
    
    // 初始化词法分析器
    init_scanner(input);
    
    printf("开始词法分析...\n");
    printf("────────────────────────────────────────\n");
    
    Token token;
    int token_count = 0;
    
    // 逐个获取并打印Token
    do {
        token = get_next_token();
        token_count++;
        
        // 打印到控制台
        print_token(token);
        
        // 写入到文件
        fprintf(output, "Line %3d, Col %3d: %-10s", 
                token.line, token.column, 
                token_type_to_string(token.type));
        
        if (token.type == TK_ID || token.type == TK_NUM || token.type == TK_STR) {
            fprintf(output, "  '%s'", token.lexeme);
        }
        fprintf(output, "\n");
        
    } while (token.type != TK_EOF && token.type != TK_ERROR);
    
    printf("────────────────────────────────────────\n");
    printf("词法分析完成！\n");
    printf("总共识别了 %d 个Token\n\n", token_count);
    
    if (token.type == TK_ERROR) {
        printf("⚠️  发现错误：%s\n", token.lexeme);
    }
    
    // 清理资源
    close_scanner();
    fclose(input);
    fclose(output);
    
    printf("结果已保存到: %s\n", output_file);
    printf("\n========================================\n");
    
    return 0;
}