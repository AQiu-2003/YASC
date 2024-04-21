//
// Created by AQiu on 2024/4/20.
//

#ifndef YASC_UTILS_H
#define YASC_UTILS_H

#include <stdbool.h>
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
#endif //YASC_UTILS_H
