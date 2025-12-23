#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main() {
    printf("========================================\n");
    printf("      实验三：LL(1)语法分析器\n");
    printf("========================================\n\n");
    
    const char* grammar_file = "grammar.txt";
    const char* input_file = "input.txt";
    const char* output_file = "ll1_result.txt";
    
    printf("文法文件: %s\n", grammar_file);
    printf("输入文件: %s\n", input_file);
    printf("输出文件: %s\n\n", output_file);
    
    // 初始化LL(1)分析器
    init_ll1_parser();
    
    // 加载文法
    load_grammar(grammar_file);
    
    // 构建FIRST和FOLLOW集
    build_first_sets();
    build_follow_sets();
    
    // 构建LL(1)分析表
    build_ll1_table();
    
    // 打印分析表
    print_analysis_table();
    
    // 执行语法分析
    parse_input(input_file);
    
    // 显示分析过程
    display_ll1_process();
    
    // 保存结果
    save_ll1_result(output_file);
    
    // 清理资源
    free_ll1_resources();
    
    printf("\n========================================\n");
    printf("        LL(1)语法分析完成！\n");
    printf("========================================\n");
    
    return 0;
}