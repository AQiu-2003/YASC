//
// Created by AQiu on 2024/4/21.
//

#include <stdio.h>
#include <stdarg.h>
#include "symbol.h"
#include "main.h"


tnode findChildByName(tnode val, char *name) {
    if(val == NULL) return NULL;
    for (int i = 0; i < val->childCount; i++) {
        if (!strcmp(val->child[i]->name, name)) {
            return val->child[i];
        }
    }
    return NULL;
}

/*
 * VarIdList:ID VarIdMore
 * VarIdMore: ε
 *      |COMMA VarIdList
 */
int moreToArray(tnode list, char *resName, char *moreName, tnode res[]) {
    int i = 0;
    if(list==NULL){
        return i;
    }
    char *baseName = list->name;
    while(findChildByName(list, resName) != NULL) {
        res[i++] = findChildByName(list, resName);
        list = findChildByName(list, moreName);
        list = findChildByName(list, baseName);
    }
    return i;
}

int moreListToArray(tnode list, char *moreName, tnode res[]) {
    int i = 0;
    char *baseName = list->name;
    while(list != NULL) {
        res[i++] = list;
        list = findChildByName(list, moreName);
        list = findChildByName(list, baseName);
    }
    return i;
}

void getAstNodeForDebug(tnode node) {
    if(node == NULL) {
        printf("NULL\n");
        return;
    }
    printf("name: %s\n", node->name);
    printf("line: %d\n", node->line);
    printf("childCount: %d\n", node->childCount);
//    printf("value: %s\n", node->value.content);
    printf("type: %s\n", node->type);
//    printf("father: %s\n", node->father->name);
//    printf("child: ");
//    for (int i = 0; i < node->childCount; i++) {
//        printf("%s ", node->child[i]->name);
//    }
    printf("\n");
}

tnode getAstNodeByPath(tnode start, unsigned int num, ...) {
    va_list ap;
    va_start(ap, num);
    tnode temp = start;
    char *nextStep = NULL;
    for (int i = 0; i < num; ++i) {
        nextStep = va_arg(ap, char *);
        temp = findChildByName(temp, nextStep);
        if(temp == NULL) break;
    }
    va_end(ap);
    return temp;
}
