//
// Created by AQiu on 2024/4/20.
//

#ifndef YASC_SYMBOL_H
#define YASC_SYMBOL_H

#include <stdbool.h>

/**********************语义分析**************************/
// 分析语法树，建立符号表
extern void initSymbol();
extern int moreToArray(tnode list, char *resName, char *moreName, tnode res[]);

// 建立类型变量符号
typedef struct type_ Type;
typedef struct record_ {
    char *name;
    Type *type;
    struct record_ *next;
} Record;
typedef struct array_ {
    unsigned int low, top;
    Type *baseType;
} Array;
struct type_ {
    char *name;
    // base(integer|char), record, array
    enum {
        integer, character, record, array
    } type;
    union {
        Record *record;
        Array *array;
    };
};

// 类型符号表的结点
typedef struct typeNode_ {
    Type *value;
    struct typeNode_ *next;
} TypeNode;
// 变量符号表的结点
typedef struct varNode_ {
    char *name;
    Type *type;
    struct varNode_ *next;
} VarNode;
// 过程符号表的结点
typedef struct procNode_ {
    char *name;
    unsigned int paramNum;           //记录函数形参个数
    VarNode *params;   //记录函数形参类型
    struct procNode_ *next;
} ProcNode;


extern VarNode *newVar(char *name, tnode decType);
extern VarNode *findVar(char *name, VarNode *from);
extern Record * newRecord(tnode decList);
extern Array *newArray(tnode arrayType);
extern TypeNode *newType(char *name, tnode decType);
extern Type *findType(char *name, TypeNode *from);
extern ProcNode *newProc(char *name, tnode paramList);
extern ProcNode *findProc(char *name, ProcNode *from);

enum symbolType_ {
    var, type, proc
};
extern bool addSymbol(char *name, tnode node, enum symbolType_ symbolType);
#endif //YASC_SYMBOL_H
