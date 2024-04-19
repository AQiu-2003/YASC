//
// Created by AQiu on 2024/4/20.
//
#include <stdio.h>
#include "main.h"
#include "analyzer.h"

/**********************语义分析**************************/
// 分析语法树，建立符号表
void analysis(tnode val) {
    int i;

}

var *varhead, *vartail;

// 建立变量符号
void newvar(int num, ...) {
    va_list valist;
    va_start(valist, num);
    var *res = (var *) malloc(sizeof(var));
    tnode temp = (tnode) malloc(sizeof(tnode));
    temp = va_arg(valist, tnode);//var type id;
    res->type = temp->value.content;
    temp = va_arg(valist, tnode);
    res->name = temp->value.content;
    vartail->next = res;
    vartail = res;
}

// 变量是否已经定义
int findvar(tnode val) {
    var *temp = (var *) malloc(sizeof(var *));
    temp = varhead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->value.content)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// 变量类型
char *typevar(tnode val) {
    var *temp = (var *) malloc(sizeof(var *));
    temp = varhead->next;
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

type *typehead, *typetail;

// 建立类型符号表
void newtype(int num, ...){
    va_list valist;
    va_start(valist,num);
    type* res=(type*)malloc(sizeof(type));
    tnode temp=(tnode)malloc(sizeof(tnode));
    temp=va_arg(valist,tnode);
    res->type=temp->value.content;
    temp=va_arg(valist,tnode);
    res->name=temp->value.content;

    typetail->next=res;
    typetail=res;
}
// 查询是否已经定义
int findtype(tnode val){

    type *temp=(type *)malloc(sizeof(type*));
    temp=typehead->next;
    while (temp!=NULL)
    {
        if(!strcmp(temp->name,val->value.content)){
            return 1;
        }
        temp=temp->next;
    }
    while (temp!=NULL)
    {
        if(!strcmp(temp->type,val->value.content)){
            return 1;
        }
        temp=temp->next;
    }
    return 0;
}
char *typetype(tnode val){
    type* temp=(type*)malloc(sizeof(type*));
    temp=typehead->next;
    while (temp!=NULL)
    {
        if(!strcmp(temp->name,val->value.content)){
            return temp->type;
        }
        temp=temp->next;
    }
    return NULL;
}

func *funchead, *functail;
// 记录函数实参
int va_num;
char *va_type[10];
// 函数实际返回值类型
char *rtype[10];

//void getargs(tnode Args);//获取实参
int checkrtype(tnode ID, tnode Args)//检查形参与实参是否一致
{
    int i;
    va_num = 0;
    getretype(Args);
    func *temp = (func *) malloc(sizeof(func *));
    temp = funchead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1) {
        if (!strcmp(temp->name, ID->value.content))
            break;
        temp = temp->next;
    }
    if (va_num != temp->va_num)
        return 1;
    for (i = 0; i < temp->va_num; i++) {
        if (temp->va_type[i] == NULL || va_type[i] == NULL || strcmp(temp->va_type[i], va_type[i]) != 0) {
            return 1;
        }
    }
    return 0;
}

// 建立函数符号
void newfunc(int num, ...) {
    int i;
    va_list valist;
    va_start(valist, num);
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    switch (num) {
        case 1: //procedure A()
            temp = va_arg(valist, tnode);
            functail->name = temp->value.content;
            functail->tag = 1;
            func *new = (func *) malloc(sizeof(func));
            functail->next = new;
            functail = new;
            break;
        case 2: // procedure A(prarmlist)
            temp = va_arg(valist, tnode);
            functail->name = temp->value.content;
            temp = va_arg(valist, tnode);
            functail->va_num = 0;
            getdetype(temp);
            break;
        default:
            break;
    }
}

void getdetype(tnode val) {//定义的参数
    int i;
    if (val != NULL) {
        if (!strcmp(val->name, "Param")) {
            functail->va_type[functail->va_num] = val->value.content;
            functail->va_num++;
            return;
        }
        for (i = 0; i < val->childCount; i++) {
            getdetype(val->childs[i]);
        }
    } else
        return;
}

void getretype(tnode val)//实际的参数
{
    int i;
    if (val != NULL) {
        if (!strcmp(val->name, "ActParamList")) {
            va_type[va_num] = val->value.content;
            va_num++;
        }
        for (i = 0; i < val->childCount; i++) {
            getretype(val->childs[i]);
        }
    }
}

// 函数是否已经定义
int findfunc(tnode val) {
    func *temp = (func *) malloc(sizeof(func *));
    temp = funchead->next;
    while (temp != NULL && temp->name != NULL && temp->tag == 1) {
        if (!strcmp(temp->name, val->value.content)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// 函数的形参个数
int numfunc(tnode val) {
    func *temp = (func *) malloc(sizeof(func *));
    temp = funchead->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, val->value.content)) {
            return temp->va_num;
        }
        temp = temp->next;
    }
    return 0;
}


array *arrayhead, *arraytail;

// 建立数组符号
void newarray(int num, ...) {
    va_list valist;
    va_start(valist, num);
    array *res = (array *) malloc(sizeof(array));
    tnode temp = (tnode) malloc(sizeof(struct treeNode));
    //array a[1..10] of basetype; a 1 10 type newarray(2,$2,$8)
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
int findarray(tnode val) {
    array *temp = (array *) malloc(sizeof(array *));
    temp = arrayhead->next;
    while (temp != NULL) {
        /* code */
        if (!strcmp(temp->name, val->value.content))
            return 1;
        temp = temp->next;
    }
    return 0;
}

// 数组类型
char *typearray(tnode val) {
    array *temp = (array *) malloc(sizeof(array *));
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