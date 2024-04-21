//
// Created by AQiu on 2024/4/20.
//

#ifndef YASC_SYMBOL_H
#define YASC_SYMBOL_H

#include <stdbool.h>
#include "main.h"

typedef struct type_ Type;
typedef struct declare_ Declare;
/**********************语义分析**************************/
// 分析语法树，建立符号表
extern void initSymbol();
extern Declare *currentScope;

// 建立类型变量符号
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
    Declare *innerDeclare; //记录函数内部类型
    struct procNode_ *next;
} ProcNode;
struct declare_ {
    struct declare_ *parent;
    TypeNode *typeHead, *typeTail;
    VarNode *varHead, *varTail;
    ProcNode *procHead, *procTail;
};


extern VarNode *newVar(char *name, tnode decType);
extern VarNode *findVar(char *name, VarNode *from);
extern Record * newRecord(tnode decList);
extern Array *newArray(tnode arrayType);
extern TypeNode *newType(char *name, tnode decType);
extern Type *findType(char *name, Declare *scope);
extern Type *findTypeInAllScope(char *name);
extern ProcNode *newProc(char *name, tnode paramList);
extern ProcNode *findProc(char *name, ProcNode *from);

Declare *initNewDeclare();
extern Declare *newDeclare(tnode declarePart);

enum symbolType_ {
    var, type, proc
};
extern bool addSymbol(char *name, tnode node, enum symbolType_ symbolType);
#endif //YASC_SYMBOL_H
