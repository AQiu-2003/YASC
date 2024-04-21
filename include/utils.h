//
// Created by AQiu on 2024/4/20.
//

#ifndef YASC_UTILS_H
#define YASC_UTILS_H

#include <stdbool.h>
#include "main.h"

extern tnode findChildByName(tnode val, char *name);
extern int moreToArray(tnode list, char *resName, char *moreName, tnode res[]);
extern int moreListToArray(tnode list, char *moreName, tnode res[]);
extern void getAstNodeForDebug(tnode node);
extern tnode getAstNodeByPath(tnode start, unsigned int num, ...);

#endif //YASC_UTILS_H
