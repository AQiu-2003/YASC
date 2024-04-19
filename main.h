#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>  // 变长参数函数 头文件

// 行数
extern int yylineno;
// 文本
extern char* yytext;
// 错误处理
void yyerror(char *msg);

void yyrestart (FILE *);

// 抽象语法树
typedef struct treeNode{
    // 行数
    int line;
    // Token类型
    char* name;
    // fchild是第一个孩子节点，next是兄弟节点，使用孩子兄弟表示法
    struct treeNode *father, *fchild, *next;

    char *content;
    int ncld;
    // 联合体，同一时间只能保存一个成员的值，分配空间是其中最大类型的内存大小
    union{
        // id内容或者type类型（int/float）
        char* id_type;
        // 具体的数值
        int intval;
        char charval;
    };
}* Ast,* tnode;

// 构造抽象语法树(节点)
Ast newAst(char* name,int num,...);

// 先序遍历语法树
void Preorder(Ast ast,int level);

// 所有节点数量
extern int nodeNum;
// 存放所有节点
extern tnode nodeList[5000];

// bison是否有词法语法错误
extern int hasFault;

/**********************语义分析**************************/
// 分析语法树，建立符号表
extern void analysis(tnode val);

// 变量符号表的结点
typedef struct var_
{
    char *name;
    char *type;
    struct var_ *next;
}var;
extern  var *varhead, *vartail;
// 建立变量符号
extern void newvar(int num,...);
// 变量是否已经定义
extern int findvar(tnode val);
// 变量类型
extern char* typevar(tnode val);
// 这样赋值号左边仅能出现ID、Exp LB Exp RB 以及 Exp DOT ID
extern int checkleft(tnode val);


// 自定义类型符号表
typedef struct type_
{
    /* data */
    char *name;
    char *type;
    struct type_ *next;
}type;
extern type *typehead,*typetail;
// 建立类型符号表
extern void newtype(int num,...);
// 查询是否已经定义
extern int findtype(tnode val);
extern char *typetype(tnode val);


// 函数符号表的结点
typedef struct func_
{
    int tag; //0表示未定义，1表示定义
    char *name;
    //char *type;
    //char *rtype; //声明返回值类型
    int va_num;  //记录函数形参个数
    char *va_type[10];
    struct func_ *next;
}func;
extern func *funchead,*functail;
// 记录函数实参
extern int va_num;
extern char* va_type[10];
extern void getdetype(tnode val);//定义的参数
extern void getretype(tnode val);//实际的参数
extern void getargs(tnode Args);//获取实参
extern int checkrtype(tnode ID,tnode Args);//检查形参与实参是否一致
// 建立函数符号
extern void newfunc(int num, ...);
// 函数是否已经定义
extern int findfunc(tnode val);
// 函数类型
extern char *typefunc(tnode val);
// 函数的形参个数
extern int numfunc(tnode val);
// 函数实际返回值类型
extern char *rtype[10];
extern int rnum;
extern void getrtype(tnode val);

// 数组符号表的结点
typedef struct array_
{
    char *name;
    char *type;
    int low;
    int top;
    struct array_ *next;
}array;
extern array *arrayhead,*arraytail;
// 建立数组符号
extern void newarray(int num, ...);
// 查找数组是否已经定义
extern int findarray(tnode val);
// 数组类型
extern char *typearray(tnode val);
