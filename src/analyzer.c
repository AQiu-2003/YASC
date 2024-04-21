//
// Created by AQiu on 2024/4/21.
//

#include <stdio.h>
#include "symbol.h"
#include "utils.h"


void analyzeDeclarePart(tnode declarePart) {
    tnode list = getAstNodeByPath(declarePart, 3, "TypeDecpart", "TypeDec", "TypeDecList");
    if (list != NULL) {
        tnode types[16];
        int count = moreListToArray(list, "TypeDecMore", types);
        for (int i = 0; i < count; i++) {
            addSymbol(types[i]->childs[0]->childs[0]->value.content, types[i]->childs[2], type);
        }
    }
    list = getAstNodeByPath(declarePart, 3, "VarDecpart", "VarDec", "VarDecList");
    if (list != NULL) {
        tnode vars[16];
        int count = moreListToArray(list, "VarDecMore", vars);
        for (int i = 0; i < count; i++) {
            tnode varList = vars[i]->childs[1];
            tnode varIds[16];
            int varCount = moreToArray(varList, "ID", "VarIdMore", varIds);
            for (int j = 0; j < varCount; j++) {
                addSymbol(varIds[j]->value.content, vars[i]->childs[0], var);
            }
        }
    }
    list = getAstNodeByPath(declarePart, 2, "ProcDecpart", "ProcDec");
    if (list != NULL) {
        tnode procs[16];
        int count = moreListToArray(list, "ProcDecMore", procs);
        for (int i = 0; i < count; i++) {
            Declare *parentScope = currentScope;
            currentScope = initNewDeclare();
            currentScope->parent = parentScope;
            addSymbol(procs[i]->childs[1]->childs[0]->value.content, procs[i]->childs[3], proc);
            analyzeDeclarePart(procs[i]->childs[6]->childs[0]);
            currentScope = parentScope;
        }
    }
}

void startAnalysis() {
    initSymbol();
    tnode globeDeclare = findChildByName(programNode, "DeclarePart");
    analyzeDeclarePart(globeDeclare);
}



#include "analyzer.h"
