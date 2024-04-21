//
// Created by AQiu on 2024/4/21.
//

#include <stdio.h>
#include "analyzer.h"
#include "symbol.h"
#include "utils.h"

bool checkIfVarDefined(tnode id) {
    VarNode *temp = findVarInAllScope(id->value.content);
    if (temp == NULL) {
        printf("Segmentation fault [%d]: variable %s not defined.\n", id->line, id->value.content);
        return false;
    }
    return true;
}

/*
 * Variable:ID VariMore
 * VariMore: ε
 *          |LMIDPAREN Exp RMIDPAREN
 *          |DOT FieldVar
 */
Type *getVariableType(tnode var) {
    Type *res = findTypeInAllScope(var->child[0]->value.content);
    if (res == NULL) {
        printf("Segmentation fault [%d]: variable %s not defined.\n", var->child[0]->line, var->child[0]->value.content);
        return NULL;
    }
    if (res->type == integer || res->type == character) {
        if(var->child[1]->childCount != 0) {
            printf("Segmentation fault [%d]: variable %s is not an array or record.\n", var->child[0]->line, var->child[0]->value.content);
            return NULL;
        }
    } else if (res->type == array) {
        if(var->child[1]->childCount == 0) {
            printf("Segmentation fault [%d]: variable %s is an array.\n", var->child[0]->line, var->child[0]->value.content);
            return NULL;
        }
        Type *expType = analyzeExp(var->child[1]->child[1]);
        if (expType == NULL) return NULL;
        if (expType->type != integer) {
            printf("Segmentation fault [%d]: array index must be integer.\n", var->child[1]->child[1]->line);
            return NULL;
        }
        res = res->array->baseType;
    } else if (res->type == record) {
        if(var->child[1]->childCount == 0) {
            printf("Segmentation fault [%d]: variable %s is a record.\n", var->child[0]->line, var->child[0]->value.content);
            return NULL;
        }
        Record *temp = res->record->next;
        while(temp != NULL) {
            if(!strcmp(temp->name, var->child[1]->child[1]->child[0]->value.content)) {
                break;
            }
            temp = temp->next;
        }
        if (temp == NULL) {
            printf("Segmentation fault [%d]: field %s not defined in record %s.\n", var->child[1]->child[1]->child[0]->line, var->child[1]->child[1]->child[0]->value.content, var->child[0]->value.content);
            return NULL;
        } else {
            if(temp->type->type == array) {
                if(var->child[1]->child[1]->child[1]->childCount == 0) {
                    printf("Segmentation fault [%d]: variable %s is an array.\n", var->child[1]->child[1]->child[0]->line, var->child[1]->child[1]->child[0]->value.content);
                    return NULL;
                }
                Type *expType = analyzeExp(var->child[1]->child[1]->child[1]->child[1]);
                if (expType == NULL) return NULL;
                if (expType->type != integer) {
                    printf("Segmentation fault [%d]: array index must be integer.\n", var->child[1]->child[1]->child[1]->child[1]->line);
                    return NULL;
                }
                res = temp->type->array->baseType;
            } else {
                res = temp->type;
            }
        }
    }
    return res;
}


Type *analyzeTerm(tnode term) {
    tnode factor = term->child[0];
    tnode otherFactor = term->child[1];
    Type *res = NULL;
    if (!strcmp(factor->child[0]->name, "LPAREN")) {
        res = analyzeExp(factor->child[1]);
    } else if (!strcmp(factor->child[0]->name, "Variable")) {
        res = getVariableType(factor->child[0]);
        if(res == NULL) return NULL;
    } else if (!strcmp(factor->child[0]->name, "INTC")) {
        res = findType("integer", globalScope);
    } else if (!strcmp(factor->child[0]->name, "CHARC")) {
        res = findType("char", globalScope);
    }
    if (otherFactor->child[0] != NULL) {
        tnode multOp = otherFactor->child[0];
        tnode nextTerm = otherFactor->child[1];
        Type *nextRes = analyzeTerm(nextTerm);
        if (res == NULL || nextRes == NULL) return NULL;
        if (res->type != integer || nextRes->type != integer) {
            printf("Segmentation fault [%d]: multiplication must be between integers.\n", multOp->line);
            return NULL;
        }
    }
    return res;
}

/*
 * Exp:Term OtherTerm{$$=newAst("Exp",2,$1,$2);}
 * OtherTerm: ε
 *          |AddOp Exp
 * Term:Factor OtherFactor
 * OtherFactor: ε
 *          |MultOP Term
 * Factor:LPAREN Exp RPAREN
 *          |INTC
 *          |CHARC
 *          |Varible
 */
Type *analyzeExp(tnode exp) {
    tnode term = exp->child[0];
    tnode otherTerm = exp->child[1];
    Type *res = analyzeTerm(term);
    if (otherTerm->child[0] != NULL) {
        tnode addOp = otherTerm->child[0];
        tnode nextExp = otherTerm->child[1];
        Type *nextRes = analyzeExp(nextExp);
        if (res == NULL || nextRes == NULL) return NULL;
        if (res->type != integer || nextRes->type != integer) {
            printf("Segmentation fault [%d]: addition must be between integers.\n", addOp->line);
            return NULL;
        }
    }
    return res;
}

void analyzeStatement(tnode stm) {
    char *stmName = stm->child[0]->name;
    if (!strcmp(stmName, "ConditionalStm")) {
        // ConditionalStm:IF RelExp THEN StmList ELSE StmList FI
        // RelExp:Exp OtherRelE

    } else if (!strcmp(stmName, "LoopStm")) {
        // LoopStm:WHILE RelExp DO StmList ENDWH
        // RelExp:Exp OtherRelE
        // OtherRelE:CmpOp Exp

    } else if (!strcmp(stmName, "InputStm")) {
        tnode id = stm->child[0]->child[2]->child[0];
        checkIfVarDefined(id);
    } else if (!strcmp(stmName, "OutputStm")) {
        if (analyzeExp(stm->child[0]->child[2]) == NULL) return;
    } else if (!strcmp(stmName, "ReturnStm")) {
        return;
    } else if (!strcmp(stmName, "ID")) {    // Stm: ID AssCall
        // AssignmentRest:VariMore ASSIGN Exp

        // CallStmRest:LPAREN ActParamList RPAREN

    }
}


void analyzeStmList(tnode stmList) {
    tnode stms[64];
    int count = moreToArray(stmList, "Stm", "StmMore", stms);
    for (int i = 0; i < count; ++i) {
        analyzeStatement(stms[i]);
    }
}

void analyzeDeclarePart(tnode declarePart) {
    tnode list = getAstNodeByPath(declarePart, 3, "TypeDecpart", "TypeDec", "TypeDecList");
    if (list != NULL) {
        tnode types[16];
        int count = moreListToArray(list, "TypeDecMore", types);
        for (int i = 0; i < count; i++) {
            addSymbol(types[i]->child[0]->child[0]->value.content, types[i]->child[2], type);
        }
    }
    list = getAstNodeByPath(declarePart, 3, "VarDecpart", "VarDec", "VarDecList");
    if (list != NULL) {
        tnode vars[16];
        int count = moreListToArray(list, "VarDecMore", vars);
        for (int i = 0; i < count; i++) {
            tnode varList = vars[i]->child[1];
            tnode varIds[16];
            int varCount = moreToArray(varList, "ID", "VarIdMore", varIds);
            for (int j = 0; j < varCount; j++) {
                addSymbol(varIds[j]->value.content, vars[i]->child[0], var);
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
            addSymbol(procs[i]->child[1]->child[0]->value.content, procs[i]->child[3], proc);
            analyzeDeclarePart(procs[i]->child[6]->child[0]);
            analyzeStmList(procs[i]->child[7]->child[0]->child[1]);
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
