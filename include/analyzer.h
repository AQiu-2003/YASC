//
// Created by AQiu on 2024/4/20.
//

#ifndef YASC_ANALYZER_H
#define YASC_ANALYZER_H

#include <stdbool.h>

/**********************语义分析**************************/
// 分析语法树，建立符号表
extern void analysis(tnode val);

// 变量符号表的结点
typedef struct var_ {
    char *name;
    char *type;
    struct var_ *next;
} Var;
extern Var *varHead, *varTail;
// 自定义类型符号表
typedef struct type_ {
    char *name;
    char *type;
    struct type_ *next;
} Type;
extern Type *typeHead, *typeTail;
// 函数符号表的结点
typedef struct proc_ {
    int tag; //0表示未定义，1表示定义
    char *name;
    //char *type;
    //char *rtype; //声明返回值类型
    int va_num;  //记录函数形参个数
    char *va_type[10];
    struct proc_ *next;
} Proc;
extern Proc *procHead, *procTail;
// 数组符号表的结点
typedef struct array_ {
    char *name;
    char *type;
    int low;
    int top;
    struct array_ *next;
} Array;
extern Array *arrayhead, *arraytail;

// 建立变量符号
extern void newvar(char * name, char * type);

// 变量是否已经定义
extern int findvar(tnode val);

// 变量类型
extern char *typevar(tnode val);

// 这样赋值号左边仅能出现ID、Exp LB Exp RB 以及 Exp DOT ID
extern int checkleft(tnode val);

// 建立类型符号表
extern void newtype(int num, ...);

// 查询是否已经定义
extern int findtype(tnode val);

extern char *typetype(tnode val);

// 记录函数实参
extern int va_num;
extern char *va_type[10];

extern void getParam(tnode val);//定义的参数
extern void getArgs(tnode val);//实际的参数
extern void getargs(tnode Args);//获取实参
extern int checkrtype(tnode param, tnode args);//检查形参与实参是否一致
// 建立函数符号
extern void newProc(int num, ...);

// 函数是否已经定义
extern int findfunc(tnode val);

// 函数类型
extern char *typefunc(tnode val);

// 函数的形参个数
extern int paramCountOfProc(tnode val);

// 函数实际返回值类型
extern char *rtype[10];
extern int rnum;

extern void getrtype(tnode val);

// 建立数组符号
extern void newArray(int num, ...);

// 查找数组是否已经定义
extern bool findArray(tnode val);

// 数组类型
extern char *typearray(tnode val);



#endif //YASC_ANALYZER_H
