//
// Created by AQiu on 2024/4/20.
//
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "symbol.h"
#include "utils.h"

/**********************语义分析**************************/
// 分析语法树，建立符号表
VarNode *varHead, *varTail;
TypeNode *typeHead, *typeTail;
ProcNode *procHead, *procTail;

void initSymbol() {
    // 初始化类型符号表
    typeHead = (TypeNode *) malloc(sizeof(TypeNode));
    typeHead->value = NULL;
    typeHead->next = NULL;
    typeTail = typeHead;
    // 初始化变量符号表
    varHead = (VarNode *) malloc(sizeof(VarNode));
    varHead->name = "head";
    varHead->type = NULL;
    varHead->next = NULL;
    varTail = varHead;
    // 初始化过程符号表
    procHead = (ProcNode *) malloc(sizeof(ProcNode));
    procHead->name = "head";
    procHead->paramNum = 0;
    procHead->params = NULL;
    procHead->next = NULL;
    procTail = procHead;

    // 建立基本类型符号（intType, char）
    Type *intType = (Type *) malloc(sizeof(Type));
    intType->name = "integer";
    intType->type = integer;
    typeTail->next = (TypeNode *) malloc(sizeof(TypeNode));
    typeTail = typeTail->next;
    typeTail->value = intType;
    typeTail->next = NULL;
    Type *charType = (Type *) malloc(sizeof(Type));
    charType->name = "char";
    charType->type = character;
    typeTail->next = (TypeNode *) malloc(sizeof(TypeNode));
    typeTail = typeTail->next;
    typeTail->value = charType;
    typeTail->next = NULL;
}

// 建立变量符号
// call from: VarDecList
VarNode *newVar(char *name, tnode decType) {
    VarNode *res = (VarNode *) malloc(sizeof(VarNode));
    res->name = name;
    res->next = NULL;
    if(!strcmp(decType->childs[0]->name, "StructureType")) {
        char *newName = (char *) malloc(sizeof(char) * 16);
        sprintf(newName, "%s-struct-%d", name, rand());
        res->type = newType(newName, decType)->value;
    } else {
        // custom type or base type
        Type *search = NULL;
        decType = decType->childs[0];
        if(!strcmp(decType->name, "ID"))
            search = findType(decType->value.content, typeHead);
        else
            search = findType(decType->childs[0]->value.content, typeHead);
        if (search == NULL) {
            printf("Error: Type %s not defined.\n", decType->childs[0]->value.content);
            return NULL;
        } else {
            res->type = search;
        }
    }
    return res;
}

// 变量是否已经定义
// call from: Exp
VarNode *findVar(char *name, VarNode *from) {
    VarNode *temp = from->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, name)) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

Record * newRecord(tnode decList) {
    // FieldDecList:BaseType IdList SEMI FieldDecMore
    Record *resHead = (Record *) malloc(sizeof(Record));
    Record *resTail = resHead;
    strcpy(resHead->name, "head");
    resHead->next = NULL;
    while (decList != NULL) {
        tnode ids[10];
        char *baseType = decList->childs[0]->childs[0]->value.content; // DecList -> BaseType -> INTEGER/CHAR
        int idCount = moreToArray(decList->childs[1], "ID", "IdMore", ids);
        Record *temp = resTail;
        for (int i = 0; i < idCount; i++) {
            temp->next = (Record *) malloc(sizeof(Record));
            temp = temp->next;
            temp->name = ids[i]->value.content;
            temp->type = findType(baseType, typeHead);
        }
        temp->next = NULL;
        decList = decList->childs[3]->childs[0];
        resTail = temp;
    }
    return resHead;
}

Array *newArray(tnode arrayType) {
    //ArrayType:ARRAY LMIDPAREN Low UNDERANGE Top RMIDPAREN OF BaseType
    Array *res = (Array *) malloc(sizeof(Array));
    res->low = arrayType->childs[2]->childs[0]->value.intValue;
    res->top = arrayType->childs[4]->childs[0]->value.intValue;
    res->baseType = findType(arrayType->childs[7]->childs[0]->value.content, typeHead);
    return res;
}


// 建立类型符号表
// call from: TypeDecList
TypeNode *newType(char *name, tnode decType) {
    struct type_ *res = (Type *) malloc(sizeof(Type));
    res->name = name;
    tnode temp = decType->childs[0];
    if (!strcmp(temp->name, "BaseType")) {
        temp = temp->childs[0]; // TypeDef -> BaseType -> INTEGER/CHAR
        if (!strcmp(temp->name, "INTEGER")) res->type = integer;
        if (!strcmp(temp->name, "CHAR")) res->type = character;
    } else if (!strcmp(temp->name, "StructureType")) {
        temp = temp->childs[0]; // TypeDef -> StructureType -> RecType/ArrayType
        if (!strcmp(temp->name, "RecType")) {
            res->type = record;
            res->record = newRecord(temp->childs[1]); // RecType -> FiledDecList
        } else if (!strcmp(temp->name, "ArrayType")) {
            res->type = array;
            res->array = newArray(temp);
        }
    } else if (!strcmp(temp->name, "ID")) {
        // custom type
        Type *search = findType(temp->value.content, typeHead);
        if(search == NULL) {
            fprintf(stderr, "Segmentation fault: %s not defined\n", temp->value.content);
            return NULL;
        } else {
            res->type = search->type;
            if (res->type == record) res->record = search->record;
            if (res->type == array) res->array = search->array;
        }
    }
    TypeNode *resNode = (TypeNode *) malloc(sizeof(TypeNode));
    resNode->value = res;
    resNode->next = NULL;
    return resNode;
}

// 查询是否已经定义
// call from: TypeDef
Type *findType(char *name, TypeNode *from) {
    TypeNode *temp = from->next;
    while (temp != NULL) {
        if (!strcmp(temp->value->name, name)) {
            return temp->value;
        }
        temp = temp->next;
    }
    return NULL;
}

// 建立函数符号
// call from: ProcDec
ProcNode *newProc(char *name, tnode paramList) {
    // ProcDec:PROCEDURE ProcName LPAREN ParamList RPAREN SEMI ProcDecPart ProcBody ProcDecMore
    ProcNode *res = (ProcNode *) malloc(sizeof(ProcNode));
    res->name = name;
    res->next = NULL;
    if(paramList->childs[0] == NULL) {
        res->paramNum = 0;
        res->params = NULL;
    } else {
        tnode params[10];
        res->paramNum = moreToArray(paramList->childs[0], "Param", "ParamMore", params);
        VarNode *paramNode = (VarNode *) malloc(sizeof(VarNode));
        paramNode->name = "param";
        paramNode->type = NULL;
        paramNode->next = NULL;
        res->params = paramNode;
        for (int i = 0; i < res->paramNum; ++i) {
            tnode decType = findChildByName(params[i], "TypeDef");
            tnode ids[10];
            int idCount = moreToArray(findChildByName(params[i], "FromList"), "ID", "FidMore", ids);
//            if (findChildByName(params[i], "VAR"))
            for (int j = 0; j < idCount; ++j) {
                VarNode *temp = newVar(ids[j]->value.content, decType);
                paramNode->next = temp;
                paramNode = temp;
            }
        }
    }
    return res;
}

// 函数是否已经定义
// call from: ID AssCall
ProcNode *findProc(char *name, ProcNode *from) {
    ProcNode *temp = from->next;
    while (temp != NULL) {
        if (!strcmp(temp->name, name)) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

bool addSymbol(char *name, tnode node, enum symbolType_ symbolType) {
    void *temp = NULL;
    switch (symbolType) {
        case var:
            temp = findVar(name, varHead);
            if ((VarNode *)temp != NULL) {
                fprintf(stderr, "Segmentation fault: %s already defined\n", name);
                return false;
            } else {
                VarNode *newVarNode = newVar(name, node);
                varTail->next = newVarNode;
                varTail = newVarNode;
            }
            break;
        case type:
            temp = findType(name, typeHead);
            if ((Type *)temp != NULL) {
                fprintf(stderr, "Segmentation fault: %s already defined\n", name);
                return false;
            } else {
                TypeNode *newTypeNode = newType(name, node);
                typeTail->next = newTypeNode;
                typeTail = newTypeNode;
            }
            break;
        case proc:
            temp = findProc(name, procHead);
            if ((ProcNode *)temp != NULL) {
                fprintf(stderr, "Segmentation fault: %s already defined\n", name);
                return false;
            } else {
                ProcNode *newProcNode = newProc(name, node);
                procTail->next = newProcNode;
                procTail = newProcNode;
            }
            break;
    }
    return true;
}

