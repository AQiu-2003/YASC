//
// Created by AQiu on 2024/4/21.
//

#ifndef YASC_ANALYZER_H
#define YASC_ANALYZER_H

#include "main.h"
#include "symbol.h"

extern void startAnalysis();
extern Type *analyzeExp(tnode exp);
void analyzeStmList(tnode stmList);

#endif //YASC_ANALYZER_H
