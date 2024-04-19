%{
#include<unistd.h>
#include<stdio.h>   
#include "main.h"
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
%type <type_tnode> Factor Varible VariMore FieldVar FieldVarMode CmpOp AddOp MultOP

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

Program:ProgramHead DeclarePart ProgramBody DOT {$$=newAst("Program",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}

ProgramHead:PROGRAM ProgramName{$$=newAst("ProgramHead",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
ProgramName:ID{$$=newAst("ProgramName",1,$1);nodeList[nodeNum]=$$;nodeNum++;}

DeclarePart:TypeDecpart VarDecpart ProcDecpart{$$=newAst("DeclarePart",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}

TypeDecpart:{$$=newAst("TypeDecpart",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |TypeDec{$$=newAst("TypeDecpart",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
TypeDec:TYPE TypeDecList{$$=newAst("TypeDec",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
TypeDecList:TypeId RELOP TypeDef SEMI TypeDecMore{$$=newAst("TypeDecList",5,$1,$2,$3,$4,$5);nodeList[nodeNum]=$$;nodeNum++;}
TypeDecMore:{$$=newAst("TypeDecMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |TypeDecList{$$=newAst("TypeDecMore",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
TypeId:ID{$$=newAst("TypeId",1,$1);nodeList[nodeNum]=$$;nodeNum++;}

TypeDef:BaseType{$$=newAst("TypeDef",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |StructureType{$$=newAst("TypeDef",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |ID{$$=newAst("TypeDef",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
BaseType:INTEGER{$$=newAst("BaseType",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |CHAR{$$=newAst("BaseType",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
StructureType:ArrayType{$$=newAst("StructureType",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    | RecType{$$=newAst("StructureType",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
//ArrayType:ARRAY '['Low'..'Top']' OF BaseType
ArrayType:ARRAY LMIDPAREN Low UNDERANGE Top RMIDPAREN OF BaseType{$$=newAst("ArrayType",8,$1,$2,$3,$4,$5,$6,$7,$8);nodeList[nodeNum]=$$;nodeNum++;}
Low:INTC{$$=newAst("Low",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
Top:INTC{$$=newAst("Top",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
RecType: RECORD FieldDecList END{$$=newAst("RecType",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
FieldDecList:BaseType IdList SEMI FieldDecMore{$$=newAst("FieldDecList",4,$1,$2,$3,$4);nodeList[nodeNum]=$$;nodeNum++;}
    |ArrayType IdList SEMI FieldDecMore{$$=newAst("FieldDecList",4,$1,$2,$3,$4);nodeList[nodeNum]=$$;nodeNum++;}
FieldDecMore:{$$=newAst("FieldDecMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |FieldDecList{$$=newAst("FieldDecMore",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
IdList:ID IdMore{$$=newAst("IdList",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
IdMore:{$$=newAst("IdMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |COMMA IdList{$$=newAst("IdMore",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}

VarDecpart:{$$=newAst("VarDecpart",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |VarDec{$$=newAst("VarDecpart",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
VarDec:VAR VarDecList{$$=newAst("VarDec",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
VarDecList:TypeDef VarIdList SEMI VarDecMore{$$=newAst("VarDecList",4,$1,$2,$3,$4);nodeList[nodeNum]=$$;nodeNum++;}
VarDecMore:{$$=newAst("VarDecMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |VarDecList{$$=newAst("VarDecMore",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
VarIdList:ID VarIdMore{$$=newAst("VarIdList",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
VarIdMore:{$$=newAst("VarIdMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |COMMA VarIdList{$$=newAst("VarIdMore",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}

ProcDecpart:{$$=newAst("ProcDecpart",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |ProcDec{$$=newAst("ProcDecpart",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
ProcDec:PROCEDURE ProcName LPAREN ParamList RPAREN SEMI ProcDecPart ProcBody ProcDecMore{$$=newAst("ProcDec",9,$1,$2,$3,$4,$5,$6,$7,$8,$9);nodeList[nodeNum]=$$;nodeNum++;}
ProcDecMore:{$$=newAst("ProcDecMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |ProcDec{$$=newAst("ProcDecMore",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
ProcName:ID{$$=newAst("ProName",1,$1);nodeList[nodeNum]=$$;nodeNum++;}

ParamList:{$$=newAst("ParamList",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |ParamDecList{$$=newAst("ParamList",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
ParamDecList:Param ParamMore{$$=newAst("ParamDecList",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
ParamMore:{$$=newAst("ParamMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |SEMI ParamDecList{$$=newAst("ParamMore",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
Param:TypeDef FormList{$$=newAst("Param",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
    |VAR TypeDef FormList{$$=newAst("Param",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
FormList:ID FidMore{$$=newAst("FormList",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
FidMore:{$$=newAst("FidMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |COMMA FormList{$$=newAst("FidMore",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}

ProcDecPart:DeclarePart{$$=newAst("ProcDecPart",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
ProcBody:ProgramBody{$$=newAst("ProcBody",1,$1);nodeList[nodeNum]=$$;nodeNum++;}

ProgramBody:BEGIN1 StmList END{$$=newAst("ProgramBody",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}

StmList:Stm {$$=newAst("StmList",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |Stm SEMI StmMore{$$=newAst("StmList",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
StmMore:{$$=newAst("StmMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |StmList{$$=newAst("StmMore",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
Stm:ConditionalStm{$$=newAst("Stm",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |LoopStm{$$=newAst("Stm",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |InputStm{$$=newAst("Stm",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |OutputStm{$$=newAst("Stm",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |ReturnStm{$$=newAst("Stm",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |ID AssCall{$$=newAst("Stm",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
AssCall:AssignmentRest{$$=newAst("AssCall",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |CallStmRest{$$=newAst("AssCall",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
AssignmentRest:VariMore ASSIGN Exp{$$=newAst("AssignmentRest",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
ConditionalStm:IF RelExp THEN StmList ELSE StmList FI{$$=newAst("ConditionalStm",7,$1,$2,$3,$4,$5,$6,$7);nodeList[nodeNum]=$$;nodeNum++;}
LoopStm:WHILE RelExp DO StmList ENDWH{$$=newAst("LoopStm",5,$1,$2,$3,$4,$5);nodeList[nodeNum]=$$;nodeNum++;}
InputStm:READ LPAREN Invar RPAREN{$$=newAst("InputStm",4,$1,$2,$3,$4);nodeList[nodeNum]=$$;nodeNum++;}
Invar:ID{$$=newAst("Invar",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
OutputStm:WRITE LPAREN Exp RPAREN{$$=newAst("OutputStm",4,$1,$2,$3,$4);nodeList[nodeNum]=$$;nodeNum++;}
ReturnStm:RETURN{$$=newAst("ReturnStm",1,$1);nodeList[nodeNum]=$$;nodeNum++;}

CallStmRest:LPAREN ActParamList RPAREN{$$=newAst("CallStmRest",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
ActParamList:{$$=newAst("ActParamList",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |Exp ActParamMore{$$=newAst("ActParamList",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
ActParamMore:{$$=newAst("ActParamMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |COMMA ActParamList{$$=newAst("ActParamMore",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
RelExp:Exp OtherRelE{$$=newAst("RelExp",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
OtherRelE:CmpOp Exp{$$=newAst("OtherRelE",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
Exp:Term OtherTerm{$$=newAst("Exp",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
OtherTerm:{$$=newAst("OtherTerm",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |AddOp Exp{$$=newAst("OtherTerm",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
Term:Factor OtherFactor{$$=newAst("Term",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
OtherFactor:{$$=newAst("OtherFactor",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |MultOP Term{$$=newAst("OtherFactor",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
Factor:LPAREN Exp RPAREN{$$=newAst("Factor",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
    |INTC{$$=newAst("Factor",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |CHARC{$$=newAst("Factor",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |Varible{$$=newAst("Factor",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
Varible:ID VariMore{$$=newAst("Varible",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
VariMore:{$$=newAst("VariMore",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |LMIDPAREN Exp RMIDPAREN{$$=newAst("VariMore",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
    |DOT FieldVar{$$=newAst("VariMore",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
FieldVar:ID FieldVarMode{$$=newAst("FieldVar",2,$1,$2);nodeList[nodeNum]=$$;nodeNum++;}
FieldVarMode:{$$=newAst("FieldVarMode",0,-1);nodeList[nodeNum]=$$;nodeNum++;}
    |LMIDPAREN Exp RMIDPAREN{$$=newAst("FieldVarMode",3,$1,$2,$3);nodeList[nodeNum]=$$;nodeNum++;}
CmpOp:RELOP{$$=newAst("CmpOp",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
AddOp:PLUS{$$=newAst("AddOp",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |MINUS{$$=newAst("AddOp",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
MultOP:TIMES{$$=newAst("MultOp",1,$1);nodeList[nodeNum]=$$;nodeNum++;}
    |DIV{$$=newAst("MultOp",1,$1);nodeList[nodeNum]=$$;nodeNum++;}

%%
