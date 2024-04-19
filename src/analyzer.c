//
// Created by AQiu on 2024/4/20.
//
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "analyzer.h"

/**********************语义分析**************************/
// 分析语法树，建立符号表
void analysis(tnode val) {
    int i;
}

Var *varHead, *varTail;

// 建立变量符号
// call from: VarDecList
void newVar(char *name, char *type) {
    Var *res = (Var *) malloc(sizeof(Var));
    res->name = name;
    res->type = type;
    varTail->next = res;
    varTail = res;
}

// 变量是否已经定义
// call from: Exp
bool findVar(tnode val) {
    Var *temp = varHead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->value.content)) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

// 变量类型
// call from: Exp
char *typeOfVar(tnode val) {
    Var *temp = varHead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->value.content)) {
            return temp->type;
        }
        temp = temp->next;
    }
    return NULL;
}

// 这样赋值号左边仅能出现ID、Exp LB Exp RB 以及 Exp DOT ID
int checkleft(tnode val);

Type *typeHead, *typeTail;

// 建立类型符号表
// call from: TypeDecList
void newType(char *name, char *type) {
    Type *res = (Type *) malloc(sizeof(Var));
    res->name = name;
    res->type = type;
    typeTail->next = res;
    typeTail = res;
}

// 查询是否已经定义
// call from: TypeDef
bool findType(tnode val) {
    Type *temp = typeHead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->value.content)) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

// 返回（自定义）类型的类型
// call from: ActParamList > Exp
char *typeOfType(tnode val) {
    Type *temp = typeHead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->value.content)) {
            return temp->type;
        }
        temp = temp->next;
    }
    return NULL;
}

Proc *procHead, *procTail;
// 记录函数实参
int va_num;
char *va_type[10];
// 函数实际返回值类型
char *rtype[10];

// param 形参 args 实参
bool checkParamMatchArgs(tnode param, tnode args) {//检查形参与实参是否一致
    int i;
    va_num = 0;
    getArgs(args);
    Proc *temp = procHead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1) {
        if (!strcmp(temp->name, param->value.content))
            break;
        temp = temp->next;
    }
    if (va_num != temp->va_num)
        return true;
    for (i = 0; i < temp->va_num; i++) {
        if (temp->va_type[i] == NULL || va_type[i] == NULL || strcmp(temp->va_type[i], va_type[i]) != 0) {
            return true;
        }
    }
    return false;
}

// 建立函数符号
// call from: ProcDec
void newProc(int num, ...) {
    int i;
    va_list valist;
    va_start(valist, num);
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    switch (num) {
        case 1: //procedure A()
            temp = va_arg(valist, tnode);
            procTail->name = temp->value.content;
            procTail->tag = 1;
            Proc *new = (Proc *) malloc(sizeof(Proc));
            procTail->next = new;
            procTail = new;
            break;
        case 2: // procedure A(param-list)
            temp = va_arg(valist, tnode);
            procTail->name = temp->value.content;
            temp = va_arg(valist, tnode);
            procTail->va_num = 0;
            getParam(temp);
            break;
        default:
            break;
    }
}

// 获取形参
// call from: ID AssCall
void getParam(tnode val) {
    int i;
    if (val != NULL) {
        if (!strcmp(val->name, "Param")) {
            procTail->va_type[procTail->va_num] = val->value.content;
            procTail->va_num++;
            return;
        }
        for (i = 0; i < val->childCount; i++) {
            getParam(val->childs[i]);
        }
    } else
        return;
}

// 获取实参
// call from: ActParamList
void getArgs(tnode val) {
    int i;
    if (val != NULL) {
        if (!strcmp(val->name, "ActParamList")) {
            va_type[va_num] = val->value.content;
            va_num++;
        }
        for (i = 0; i < val->childCount; i++) {
            getArgs(val->childs[i]);
        }
    }
}

// 函数是否已经定义
// call from: ID AssCall
int findProc(tnode val) {
    Proc *temp = (Proc *) malloc(sizeof(Proc *));
    temp = procHead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1) {
        if (!strcmp(temp->name, val->value.content)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// 函数的形参个数
// call from: ID AssCall
int paramCountOfProc(tnode val) {
    Proc *temp = (Proc *) malloc(sizeof(Proc *));
    temp = procHead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->value.content)) {
            return temp->va_num;
        }
        temp = temp->next;
    }
    return 0;
}

// 函数的实参个数
// call from: ActParamList
int argsCountOfCall(tnode val) {
    int i;
    int count = 0;
    for (i = 0; i < val->childCount; i++) {
        if (!strcmp(val->childs[i]->name, "ActParamList")) {
            count++;
        }
    }
    return count;

}


Array *arrayhead, *arraytail;

// 建立数组符号
void newArray(int num, ...) {
    va_list valist;
    va_start(valist, num);
    Array *res = (Array *) malloc(sizeof(Array));
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    //Array a[1..10] of basetype; a 1 10 type newArray(2,$2,$8)
    temp = va_arg(valist, tnode);
    res->name = temp->value.content;
    temp = va_arg(valist, tnode);
    res->low = atoi(temp->value.content);
    temp = va_arg(valist, tnode);
    res->top = atoi(temp->value.content);
    temp = va_arg(valist, tnode);
    res->type = temp->value.content;
    arraytail->next = res;
    arraytail = res;
}

// 查找数组是否已经定义
bool findArray(tnode val) {
    Array *temp = (Array *) malloc(sizeof(Array *));
    temp = arrayhead->next;
    while (temp != NULL) {
        /* code */
        if (!strcmp(temp->name, val->value.content))
            return true;
        temp = temp->next;
    }
    return false;
}

// 数组类型
char *typeOfArray(tnode val) {
    Array *temp = (Array *) malloc(sizeof(Array *));
    temp = arrayhead->next;
    while (temp != NULL) {
        /* code */
        while (temp != NULL) {
            /* code */
            if (!strcmp(temp->name, val->value.content)) {
                return temp->type;
            }
            temp = temp->next;
        }

    }
    return NULL;
}