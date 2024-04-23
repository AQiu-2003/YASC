//
// Created by AQiu on 2024/4/21.
//

#include <stdio.h>
#include "analyzer.h"
#include "symbol.h"
#include "utils.h"

bool inCall = false;

bool checkIfVarDefined(tnode id) {
    VarNode *temp = findVarInAllScope(id->value.content);
    if (temp == NULL) {
        fprintf(stderr, "Segmentation fault [line %d]: variable %s not defined.\n", id->line, id->value.content);
        return false;
    }
    return true;
}
/*
 * FieldVar:ID FieldVarMode
 * FieldVarMode: ε
 *              |LMIDPAREN Exp RMIDPAREN
 * （需要先判断id是否存在&&id是否是record类型）
 */
Type *analyzeRecord(tnode recordId, tnode fieldVar) {
    if (!checkIfVarDefined(recordId)) {
        return NULL;
    }
    VarNode *recordVar = findVarInAllScope(recordId->value.content);
    if (recordVar->type->type != record) {
        fprintf(stderr, "Segmentation fault [line %d]: variable %s is not a record.\n", recordId->line, recordId->value.content);
        return NULL;
    }
    Type *res = recordVar->type;
    tnode id = fieldVar->child[0];
    Record *temp = res->record->next;
    while(temp != NULL) {
        if(!strcmp(temp->name, id->value.content)) {
            break;
        }
        temp = temp->next;
    }
    if(temp == NULL) {
        fprintf(stderr, "Segmentation fault [line %d]: field %s not defined in record %s.\n", id->line, id->value.content, recordId->value.content);
        return NULL;
    }
    tnode fieldVarMode = fieldVar->child[1];
    if(fieldVarMode->childCount == 0) {
        if(temp->type->type == array) {
            fprintf(stderr, "Segmentation fault [line %d]: field %s is an array.\n", id->line, id->value.content);
            return NULL;
        }
        return temp->type;
    } else {
        if(temp->type->type != array) {
            fprintf(stderr, "Segmentation fault [line %d]: field %s is not an array.\n", id->line, id->value.content);
            return NULL;
        }
        Type *expType = analyzeExp(fieldVarMode->child[1]);
        if(expType == NULL) {
            return NULL;
        }
        if(expType->type != integer) {
            fprintf(stderr, "Segmentation fault [line %d]: index of array %s is not an integer.\n", id->line, id->value.content);
            return NULL;
        }
        return temp->type->array->baseType;
    }
}

Type *analyzeArray(tnode arrayId, tnode variMore) {
    if (!checkIfVarDefined(arrayId)) {
        return NULL;
    }
    VarNode *arrNode = findVarInAllScope(arrayId->value.content);
    Type *res = arrNode->type;
    if(res==NULL){
        return NULL;
    }
    if (res->type != array) {
        fprintf(stderr, "Segmentation fault [line %d]: variable %s is not an array.\n", arrayId->line, arrayId->value.content);
        return NULL;
    }
    tnode exp = variMore->child[1];
    inCall = true;
    Type *expType = analyzeExp(exp);
    inCall = false;
    if(expType == NULL) {
        return NULL;
    }
    if(expType->type != integer) {
        fprintf(stderr, "Segmentation fault [line %d]: index of array %s is not an integer.\n", arrayId->line, arrayId->value.content);
        return NULL;
    }
    //shallow check the array range
    // exp -> term
    tnode callValueNode = getAstNodeByPath(exp, 3, "Term", "Factor", "INTC");
    if (callValueNode != NULL) {
        int callValue = callValueNode->value.intValue;
        if(callValue < res->array->low || callValue > res->array->top) {
            fprintf(stderr, "Segmentation fault [line %d]: index of array %s out of range.\n", arrayId->line, arrayId->value.content);
            return NULL;
        }
    }
    return res->array->baseType;
}


/*
 * Variable:ID VariMore
 * VariMore: ε
 *          |LMIDPAREN Exp RMIDPAREN
 *          |DOT FieldVar
 */
Type *analyzeVariable(tnode var) {
    VarNode *res = findVarInAllScope(var->child[0]->value.content);
    if (res == NULL) {
        fprintf(stderr, "Segmentation fault [line %d]: variable %s not defined.\n", var->child[0]->line, var->child[0]->value.content);
        return NULL;
    }
    if (res->type->type == integer || res->type->type == character) {
        if(var->child[1]->childCount != 0) {
            fprintf(stderr, "Segmentation fault [line %d]: variable %s is not an array or record.\n", var->child[0]->line, var->child[0]->value.content);
            return NULL;
        } else return res->type;
    } else if (res->type->type == array) {
        if(var->child[1]->childCount == 0) {
            if(inCall) return res->type;
            fprintf(stderr, "Segmentation fault [line %d]: variable %s is an array.\n", var->child[0]->line, var->child[0]->value.content);
            return NULL;
        }
        Type *expType = analyzeExp(var->child[1]->child[1]);
        if (expType == NULL) return NULL;
        if (expType->type != integer) {
            fprintf(stderr, "Segmentation fault [line %d]: array index must be integer.\n", var->child[1]->child[1]->line);
            return NULL;
        }
        return res->type->array->baseType;
    } else if (res->type->type == record) {
        if(var->child[1]->childCount == 0) {
            if(inCall) return res->type;
            fprintf(stderr, "Segmentation fault [line %d]: variable %s is a record.\n", var->child[0]->line, var->child[0]->value.content);
            return NULL;
        }
        return analyzeRecord(var->child[0],var->child[1]->child[1]);
    }
    return NULL;
}


Type *analyzeTerm(tnode term) {
    tnode factor = term->child[0];
    tnode otherFactor = term->child[1];
    Type *res = NULL;
    if (!strcmp(factor->child[0]->name, "LPAREN")) {
        res = analyzeExp(factor->child[1]);
    } else if (!strcmp(factor->child[0]->name, "Variable")) {
        res = analyzeVariable(factor->child[0]);
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
            printf("Segmentation fault [line %d]: multiplication must be between integers.\n", multOp->line);
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
    if (exp == NULL) return NULL;
    tnode term = exp->child[0];
    tnode otherTerm = exp->child[1];
    Type *res = analyzeTerm(term);
    if (otherTerm->child[0] != NULL) {
        tnode addOp = otherTerm->child[0];
        tnode nextExp = otherTerm->child[1];
        Type *nextRes = analyzeExp(nextExp);
        if (res == NULL || nextRes == NULL) return NULL;
        if (res->type != integer || nextRes->type != integer) {
            printf("Segmentation fault [line %d]: addition must be between integers.\n", addOp->line);
            return NULL;
        }
    }
    return res;
}

bool analyzeRelExp(tnode relExp) {
    if(relExp == NULL){
        fprintf(stderr, "Segmentation fault: Null relational expression node in analyzeRelExp.\n");
        return false;
    }
    tnode exp1 = relExp->child[0];
    tnode exp2 = relExp->child[1]->child[1];
    Type *res1 = analyzeExp(exp1);
    Type *res2 = analyzeExp(exp2);
    if (res1 == NULL || res2 == NULL) return false;
    if (res1->type != res2->type) {
        printf("Segmentation fault [line %d]: comparison between different types.\n", relExp->child[1]->line);
        return false;
    }
    return true;
}

bool checkCallStm(tnode procID, tnode argsList) {
    ProcNode *procType = findProcInAllScope(procID->value.content);
    if (procType == NULL) {
        fprintf(stderr, "Segmentation fault [line %d]: procedure %s not defined in current scope.\n", procID->line, procID->value.content);
        return false;
    }
    VarNode *param;
    if (procType->paramNum == 0) param = NULL;
    else param = procType->params->next;
    tnode exps[10];
    int expCount = moreToArray(argsList, "Exp", "ActParamMore", exps);
    if (expCount < procType->paramNum) {
        fprintf(stderr, "Segmentation fault [line %d]: too few arguments for procedure %s, expected %d.\n", procID->line, procID->value.content, procType->paramNum);
        return false;
    } else if (expCount > procType->paramNum) {
        fprintf(stderr, "Segmentation fault [line %d]: too many arguments for procedure %s, expected %d.\n", procID->line, procID->value.content, procType->paramNum);
        return false;
    }
    for (int i = 0; i < expCount; ++i) {
        Type *expType = analyzeExp(exps[i]);
        if (expType->type != param->type->type) {
            fprintf(stderr, "Segmentation fault [line %d]: argument %d of procedure %s has wrong type.\n", exps[i]->line, i + 1, procID->value.content);
            return false;
        }
        param = param->next;
    }
    return true;
}

void analyzeStatement(tnode stm) {
    if(stm==NULL){
        fprintf(stderr, "Error: Null statement node in analyzeStatement.\n");
        return;
    }
    char *stmName = stm->child[0]->name;
    if (!strcmp(stmName, "ConditionalStm")) {
        // ConditionalStm:IF RelExp THEN StmList ELSE StmList FI
        // RelExp:Exp OtherRelE
        // OtherRelE:CmpOp Exp
        if (!analyzeRelExp(stm->child[0]->child[1])) return;
        analyzeStmList(stm->child[0]->child[3]);
        analyzeStmList(stm->child[0]->child[5]);
    } else if (!strcmp(stmName, "LoopStm")) {
        // LoopStm:WHILE RelExp DO StmList ENDWH
        // RelExp:Exp OtherRelE
        // OtherRelE:CmpOp Exp
        if (!analyzeRelExp(stm->child[0]->child[1])) return;
        analyzeStmList(stm->child[0]->child[3]);
    } else if (!strcmp(stmName, "InputStm")) {
        // InputStm:READ LPAREN Invar RPAREN
        tnode id = stm->child[0]->child[2]->child[0];
        checkIfVarDefined(id);
    } else if (!strcmp(stmName, "OutputStm")) {
        // OutputStm:WRITE LPAREN Exp RPAREN
        if (analyzeExp(stm->child[0]->child[2]) == NULL) return;
    } else if (!strcmp(stmName, "ReturnStm")) {
        return;
    } else if (!strcmp(stmName, "ID")) {    // Stm: ID AssCall(AssignmentRest/CallStmRest)
        tnode rest = stm->child[1]->child[0];
        // AssignmentRest:VariMore ASSIGN Exp
        if(!strcmp(rest->name, "AssignmentRest")) {
            VarNode *id = findVarInAllScope(stm->child[0]->value.content);
            if (id == NULL) {
                fprintf(stderr, "Segmentation fault [line %d]: variable %s not defined.\n", stm->child[0]->line, stm->child[0]->value.content);
                return;
            }
            Type *expType = analyzeExp(rest->child[2]);
            if(expType==NULL){
                return;
            }
            if(id->type->type == integer || id->type->type == character) {
                if(id->type->type != expType->type) {
                    fprintf(stderr, "Segmentation fault [line %d]: type mismatch in assignment.\n", rest->child[1]->line);
                    return;
                }
            } else {
                if(rest->child[0]->childCount == 0) {
                    fprintf(stderr, "Segmentation fault [line %d]: structure type can't be assigned directly.\n", rest->child[1]->line);
                    return;
                }
                if(id->type->type == array) {
                    Type *arrayType = analyzeArray(stm->child[0], stm->child[1]->child[0]->child[0]);
                    if(arrayType==NULL){
                        return;
                    }
                    if(arrayType->type != expType->type) {
                        fprintf(stderr, "Segmentation fault [line %d]: type mismatch in assignment.\n", rest->child[1]->line);
                        return;
                    }
                } else if(id->type->type == record) {
                    Type *fieldType = analyzeRecord(stm->child[0], stm->child[1]->child[0]->child[0]->child[1]);
                    if(fieldType==NULL){
                        return;
                    }
                    if(fieldType->type != expType->type) {
                        fprintf(stderr, "Segmentation fault [line %d]: type mismatch in assignment.\n", rest->child[1]->line);
                        return;
                    }
                }
            }
        } else if (!strcmp(rest->name, "CallStmRest")) {
            inCall = true;
            ProcNode *proc = findProcInAllScope(stm->child[0]->value.content);
            if (proc == NULL) {
                fprintf(stderr, "Segmentation fault [line %d]: procedure %s not defined.\n", stm->child[0]->line, stm->child[0]->value.content);
                return;
            }
            // CallStmRest:LPAREN ActParamList RPAREN
            if (!checkCallStm(stm->child[0], rest->child[1])) return;
            inCall = false;
        }
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
    tnode programStmList = getAstNodeByPath(programNode, 2, "ProgramBody", "StmList");
    analyzeStmList(programStmList);
    printf("YASC: Segmentation analysis finished.\n");
}