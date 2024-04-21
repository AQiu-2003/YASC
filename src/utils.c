//
// Created by AQiu on 2024/4/21.
//

#include <stdio.h>
#include "symbol.h"
#include "main.h"


tnode findChildByName(tnode val, char *name) {
    if(val == NULL) return NULL;
    for (int i = 0; i < val->childCount; i++) {
        if (!strcmp(val->childs[i]->name, name)) {
            return val->childs[i];
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
    char *baseName = list->name;
    while(findChildByName(list, resName) != NULL) {
        res[i++] = findChildByName(list, resName);
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
//    printf("childs: ");
//    for (int i = 0; i < node->childCount; i++) {
//        printf("%s ", node->childs[i]->name);
//    }
    printf("\n");
}