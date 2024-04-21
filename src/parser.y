%{
#include<unistd.h>
#include<stdio.h>   
#include "main.h"
#include "symbol.h"
#include "utils.h"
int yylex();
%}
%union{
    tnode type_tnode;
	double d;
}

/*声明记号*/
%token <type_tnode> INTC CHARC ID
%token <type_tnode> PROGRAM PROCEDURE TYPE VAR IF THEN ELSE FI WHILE DO ENDWH BEGIN1 END READ WRITE ARRAY OF RECORD RETURN INTEGER CHAR
%token <type_tnode> SEMI COMMA ASSIGN PLUS MINUS TIMES DIV DOT LPAREN RPAREN LMIDPAREN RMIDPAREN RELOP UNDERANGE
 /* %token <type_tnode> COMMENT SPACE EOL AERROR */

//总程序
%type <type_tnode> Program
//程序头
%type <type_tnode> ProgramHead ProgramName
//程序声明
%type <type_tnode> DeclarePart
//类型声明
%type <type_tnode> TypeDecpart TypeDec TypeDecList TypeDecMore TypeId
//类型
%type <type_tnode> TypeDef BaseType StructureType ArrayType Low Top RecType FieldDecList FieldDecMore IdList IdMore
//变量声明
%type <type_tnode> VarDecpart VarDec VarDecList VarDecMore VarIdList VarIdMore
//过程声明
%type <type_tnode> ProcDecpart ProcDec ProcDecMore ProcName
//参数声明
%type <type_tnode> ParamList ParamDecList ParamMore Param FormList FidMore
//过程的声明部分
%type <type_tnode> ProcDecPart
//过程体
%type <type_tnode> ProcBody
//主程序体
%type <type_tnode> ProgramBody
//语句序列
%type <type_tnode> StmList StmMore
//语句
%type <type_tnode> Stm AssCall
//赋值语句 条件语句
%type <type_tnode> AssignmentRest ConditionalStm LoopStm InputStm Invar OutputStm ReturnStm CallStmRest ActParamList ActParamMore
//表达式
%type <type_tnode> RelExp OtherRelE Exp OtherTerm
//项
%type <type_tnode> Term OtherFactor
//因子
%type <type_tnode> Factor Variable VariMore FieldVar FieldVarMode CmpOp AddOp MultOP

/*优先级*/
%right ASSIGN
%left RELOP
%left PLUS MINUS
%left TIMES DIV
%left LPAREN RPAREN LMIDPAREN RMIDPAREN DOT

%nonassoc ELSE

/*产生式*/
/*$$表示左表达式 ${num}表示右边的第几个表达式*/
%%

Program:ProgramHead DeclarePart ProgramBody DOT {
        $$=newAst("Program",3,$1,$2,$3);
        programNode = $$;
    }

ProgramHead:PROGRAM ProgramName{$$=newAst("ProgramHead",2,$1,$2);}
ProgramName:ID{$$=newAst("ProgramName",1,$1);}

DeclarePart:TypeDecpart VarDecpart ProcDecpart{$$=newAst("DeclarePart",3,$1,$2,$3);}

TypeDecpart:{$$=newAst("TypeDecpart",0,-1);}
    |TypeDec{$$=newAst("TypeDecpart",1,$1);}
TypeDec:TYPE TypeDecList{$$=newAst("TypeDec",2,$1,$2);}
TypeDecList:TypeId RELOP TypeDef SEMI TypeDecMore{
    $$=newAst("TypeDecList",5,$1,$2,$3,$4,$5);
    //addSymbol($1->child[0]->value.content, $3, type);
}
TypeDecMore:{$$=newAst("TypeDecMore",0,-1);}
    |TypeDecList{$$=newAst("TypeDecMore",1,$1);}
TypeId:ID{$$=newAst("TypeId",1,$1);}

TypeDef:BaseType{$$=newAst("TypeDef",1,$1);}
    |StructureType{$$=newAst("TypeDef",1,$1);}
    |ID{$$=newAst("TypeDef",1,$1);}
BaseType:INTEGER{$$=newAst("BaseType",1,$1);}
    |CHAR{$$=newAst("BaseType",1,$1);}
StructureType:ArrayType{$$=newAst("StructureType",1,$1);}
    | RecType{$$=newAst("StructureType",1,$1);}
ArrayType:ARRAY LMIDPAREN Low UNDERANGE Top RMIDPAREN OF BaseType{$$=newAst("ArrayType",8,$1,$2,$3,$4,$5,$6,$7,$8);}
Low:INTC{$$=newAst("Low",1,$1);}
Top:INTC{$$=newAst("Top",1,$1);}
RecType: RECORD FieldDecList END{$$=newAst("RecType",3,$1,$2,$3);}
FieldDecList:BaseType IdList SEMI FieldDecMore{$$=newAst("FieldDecList",4,$1,$2,$3,$4);}
    |ArrayType IdList SEMI FieldDecMore{$$=newAst("FieldDecList",4,$1,$2,$3,$4);}
FieldDecMore:{$$=newAst("FieldDecMore",0,-1);}
    |FieldDecList{$$=newAst("FieldDecMore",1,$1);}
IdList:ID IdMore{$$=newAst("IdList",2,$1,$2);}
IdMore:{$$=newAst("IdMore",0,-1);}
    |COMMA IdList{$$=newAst("IdMore",2,$1,$2);}

VarDecpart:{$$=newAst("VarDecpart",0,-1);}
    |VarDec{$$=newAst("VarDecpart",1,$1);}
VarDec:VAR VarDecList{$$=newAst("VarDec",2,$1,$2);}
VarDecList:TypeDef VarIdList SEMI VarDecMore {
        $$=newAst("VarDecList",4,$1,$2,$3,$4);
        //tnode vars[10];
        //int count = moreToArray($2, "ID", "VarIdMore", vars);
        //for(int i = 0; i < count; i++){
        //    addSymbol(vars[i]->value.content, $1, var);
        //}
    }
VarDecMore:{$$=newAst("VarDecMore",0,-1);}
    |VarDecList{$$=newAst("VarDecMore",1,$1);}
VarIdList:ID VarIdMore{$$=newAst("VarIdList",2,$1,$2);}
VarIdMore:{$$=newAst("VarIdMore",0,-1);}
    |COMMA VarIdList{$$=newAst("VarIdMore",2,$1,$2);}

ProcDecpart:{$$=newAst("ProcDecpart",0,-1);}
    |ProcDec{$$=newAst("ProcDecpart",1,$1);}
ProcDec:PROCEDURE ProcName LPAREN ParamList RPAREN SEMI ProcDecPart ProcBody ProcDecMore{
    $$=newAst("ProcDec",9,$1,$2,$3,$4,$5,$6,$7,$8,$9);
    //addSymbol($2->child[0]->value.content, $4, proc);
}
ProcDecMore:{$$=newAst("ProcDecMore",0,-1);}
    |ProcDec{$$=newAst("ProcDecMore",1,$1);}
ProcName:ID{
    $$=newAst("ProcName",1,$1);
    // 进入新的过程作用域
    //Declare *newScope = initNewDeclare();
    //newScope->parent = currentScope;
    //currentScope = newScope;
}

ParamList:{$$=newAst("ParamList",0,-1);}
    |ParamDecList{$$=newAst("ParamList",1,$1);}
ParamDecList:Param ParamMore{$$=newAst("ParamDecList",2,$1,$2);}
ParamMore:{$$=newAst("ParamMore",0,-1);}
    |SEMI ParamDecList{$$=newAst("ParamMore",2,$1,$2);}
Param:TypeDef FormList{$$=newAst("Param",2,$1,$2);}
    |VAR TypeDef FormList{$$=newAst("Param",3,$1,$2,$3);}
FormList:ID FidMore{$$=newAst("FormList",2,$1,$2);}
FidMore:{$$=newAst("FidMore",0,-1);}
    |COMMA FormList{$$=newAst("FidMore",2,$1,$2);}

ProcDecPart:DeclarePart{$$=newAst("ProcDecPart",1,$1);}
ProcBody:ProgramBody{$$=newAst("ProcBody",1,$1);}

ProgramBody:BEGIN1 StmList END{$$=newAst("ProgramBody",3,$1,$2,$3);}

StmList:Stm {$$=newAst("StmList",1,$1);}
    |Stm SEMI StmMore{$$=newAst("StmList",3,$1,$2,$3);}
StmMore:{$$=newAst("StmMore",0,-1);}
    |StmList{$$=newAst("StmMore",1,$1);}
Stm:ConditionalStm{$$=newAst("Stm",1,$1);}
    |LoopStm{$$=newAst("Stm",1,$1);}
    |InputStm{$$=newAst("Stm",1,$1);}
    |OutputStm{$$=newAst("Stm",1,$1);}
    |ReturnStm{$$=newAst("Stm",1,$1);}
    |ID AssCall{$$=newAst("Stm",2,$1,$2);}
AssCall:AssignmentRest{$$=newAst("AssCall",1,$1);}
    |CallStmRest{$$=newAst("AssCall",1,$1);}
AssignmentRest:VariMore ASSIGN Exp{$$=newAst("AssignmentRest",3,$1,$2,$3);}
ConditionalStm:IF RelExp THEN StmList ELSE StmList FI{$$=newAst("ConditionalStm",7,$1,$2,$3,$4,$5,$6,$7);}
LoopStm:WHILE RelExp DO StmList ENDWH{$$=newAst("LoopStm",5,$1,$2,$3,$4,$5);}
InputStm:READ LPAREN Invar RPAREN{$$=newAst("InputStm",4,$1,$2,$3,$4);}
Invar:ID{$$=newAst("Invar",1,$1);}
OutputStm:WRITE LPAREN Exp RPAREN{$$=newAst("OutputStm",4,$1,$2,$3,$4);}
ReturnStm:RETURN{$$=newAst("ReturnStm",1,$1);}

CallStmRest:LPAREN ActParamList RPAREN{$$=newAst("CallStmRest",3,$1,$2,$3);}
ActParamList:{$$=newAst("ActParamList",0,-1);}
    |Exp ActParamMore{$$=newAst("ActParamList",2,$1,$2);}
ActParamMore:{$$=newAst("ActParamMore",0,-1);}
    |COMMA ActParamList{$$=newAst("ActParamMore",2,$1,$2);}
RelExp:Exp OtherRelE{$$=newAst("RelExp",2,$1,$2);}
OtherRelE:CmpOp Exp{$$=newAst("OtherRelE",2,$1,$2);}
Exp:Term OtherTerm{$$=newAst("Exp",2,$1,$2);}
OtherTerm:{$$=newAst("OtherTerm",0,-1);}
    |AddOp Exp{$$=newAst("OtherTerm",2,$1,$2);}
Term:Factor OtherFactor{$$=newAst("Term",2,$1,$2);}
OtherFactor:{$$=newAst("OtherFactor",0,-1);}
    |MultOP Term{$$=newAst("OtherFactor",2,$1,$2);}
Factor:LPAREN Exp RPAREN{$$=newAst("Factor",3,$1,$2,$3);}
    |INTC{$$=newAst("Factor",1,$1);}
    |CHARC{$$=newAst("Factor",1,$1);}
    |Variable{$$=newAst("Factor",1,$1);}
Variable:ID VariMore{$$=newAst("Variable",2,$1,$2);}
VariMore:{$$=newAst("VariMore",0,-1);}
    |LMIDPAREN Exp RMIDPAREN{$$=newAst("VariMore",3,$1,$2,$3);}
    |DOT FieldVar{$$=newAst("VariMore",2,$1,$2);}
FieldVar:ID FieldVarMode{$$=newAst("FieldVar",2,$1,$2);}
FieldVarMode:{$$=newAst("FieldVarMode",0,-1);}
    |LMIDPAREN Exp RMIDPAREN{$$=newAst("FieldVarMode",3,$1,$2,$3);}
CmpOp:RELOP{$$=newAst("CmpOp",1,$1);}
AddOp:PLUS{$$=newAst("AddOp",1,$1);}
    |MINUS{$$=newAst("AddOp",1,$1);}
MultOP:TIMES{$$=newAst("MultOp",1,$1);}
    |DIV{$$=newAst("MultOp",1,$1);}

%%
