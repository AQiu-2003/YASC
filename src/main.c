#include <ctype.h>
#include "main.h"
#include "parser.tab.h"
#include "symbol.h"
#include "utils.h"
#include "analyzer.h"

int hasFault;
tnode programNode;

Ast newAst(char *name, int num, ...) {
    // 生成父节点
    tnode father = (tnode) malloc(sizeof(struct treeNode));
    // 添加子节点
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    if (!father) {
        yyerror("create treenode error");
        exit(0);
    }
    father->name = name;
    father->line = -1;

    // 参数列表，详见 stdarg.h 用法
    va_list list;
    // 初始化参数列表
    va_start(list, num);

    // 表示当前节点不是终结符号，还有子节点
    if (num > 0) {
        temp = va_arg(list, tnode);
        // father->line = temp->line;
        father->childCount = num;
//        if (num == 1 && isToken(temp)) {
//            father->type = temp->type;
//            father->value = temp->value;
//            father->line = temp->line;
//            // 释放temp
//            free(temp);
//        } else {
            for (int i = 0; i < num; ++i) {
                temp->father = father;
                father->child[i] = temp;
                temp = va_arg(list, tnode);
            }
            for (int i = 0; i < num; i++) {
                if (father->child[i]->line != -1) {
                    father->line = father->child[i]->line;
                    break;
                }
            }
//        }


    } else { //表示当前节点是终结符（叶节点）或者空的语法单元，此时num表示行号（空单元为-1）,将对应的值存入union
        father->line = va_arg(list, int);
        if (!strcmp(name, "INTC")) {
            father->value.intValue = atoi(yytext);
            father->type = "int";
        } else if (!strcmp(name, "CHARC")) {
            father->value.charValue = yytext[1];
            father->type = "char";
        } else {
            char *str;
            str = (char *) malloc(sizeof(char) * 40);
            strcpy(str, yytext);
            father->value.content = str;
            if (!strcmp(name, "ID")) father->type = "id";
            else if (!strcmp(name, "TYPE")) father->type = "type";
        }
    }
//    getAstNodeForDebug(father);
    va_end(list);
    return father;
}

// 父节点->左子节点->右子节点....
void Preorder(Ast ast, int level) {
    // printf(" into ");
    if (ast != NULL && ast->line != -1) {
        // 层级结构缩进
        for (int i = 0; i < level; ++i) {
            printf(" ");
        }
        // printf(" rline ");
        // printf(" prnt ");
        // 打印节点类型
        printf("%s", ast->name);
        // 根据不同类型打印节点数据
        if (ast->type != NULL) {
            printf(": %s", ast->type);
            if (!strcmp(ast->type, "int")) printf(" -> %d", ast->value.intValue);
            else if (!strcmp(ast->type, "char")) printf(" -> %c", ast->value.charValue);
            else if (!strcmp(ast->type, "id") || !strcmp(ast->type, "type")) printf(" -> %s", ast->value.content);
        } else {
            printf("(%d)", ast->line);
        }

        printf("\n");
        for (int i = 0; i < ast->childCount; i++) {
            Preorder(ast->child[i], level + 1);
        }
    }
}

// 错误处理
void yyerror(char *msg) {
    hasFault = 1;
    fprintf(stderr, "Syntax Error [Line %d]: %s, before '%s'\n", yylineno, msg, yytext);
}

// 主函数 扫描文件并且分析
// 为bison会自己调用yylex()，所以在main函数中不需要再调用它了
// bison使用yyparse()进行语法分析，所以需要我们在main函数中调用yyparse()和yyrestart()
int main(int argc, char **argv) {
    int j;
    int printTree = 1;
    printf("YASC: Start analysis...\n");
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    int i = 1;
    if (argc > 2) {
        if (strcmp(argv[1], "--no-tree") == 0) {
            printTree = 0;
            i = 2;
        }
    }
    for (; i < argc; i++) {
        // 初始化节点记录列表
        hasFault = 0;
        FILE *f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[i]);
            return 1;
        }
        printf("YASC: Start parsing file %s...\n\n", argv[i]);
        printf("YASC: Printing tokens...\n");
        yyrestart(f);
        yyparse();
        fclose(f);
        printf("\nYASC: Finish printing tokens!\n");
        // 遍历所有非子节点的节点
        if (hasFault) continue;
        else printf("YASC: No syntax error found.\n");
        startAnalysis();
        if (printTree) {
            // 刷新输出流
            fflush(stdout);
            printf("YASC: Print AST...\n\n");
            Preorder(programNode, 0);
            printf("\n\nYASC: Finish printing AST!\n");
        }
        printf("YASC: Finish parsing file %s\n", argv[i]);
    }
    printf("YASC: Finish analysis of all files\n");
}