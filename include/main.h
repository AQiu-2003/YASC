#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>  // 变长参数函数 头文件
#include <stdbool.h>

// 词法分析
extern int yylineno;
extern char *yytext;
extern bool printToken;
void yyerror(char *msg);
void yyrestart(FILE *);

// 抽象语法树 AST
typedef struct treeNode {
    // 行数
    int line;
    // Token类型
    char *name;
    // 记录子节点，最多10个，使用数组存储
    struct treeNode *father, *child[10];
    int childCount;    //记录孩子节点个数

    char *type;     //存放类型（int/char)
    union {
        int intValue;   //存放int类型的值
        char charValue;   //存放char类型的值
        char* content;  //存放语义值
    } value;
} *Ast, *tnode;

// 语法树根节点
extern tnode programNode;

// 构造抽象语法树(节点)
Ast newAst(char *name, int num, ...);

// 先序遍历语法树
void Preorder(Ast ast, int level);

// bison是否有词法语法错误
extern int hasFault;