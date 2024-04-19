%option yylineno
%option noyywrap

%{
	#include <stdlib.h>
	#include <stdio.h>
    #include "main.h"
    #include "parser.tab.h"

	#define ASTRETURN(a) yylval.type_tnode=newAst(#a,0,yylineno);return a
%}

/*无符号整数*/
INTC 0|[1-9][0-9]*
/*ASCII字符*/
CHARC \'[a-zA-Z0-9@#$%^&*()_+|~`{}:"<>?/.,';\[\]\\-]\'
/*标识符*/
ID [a-z][a-z0-9]*

/*保留字*/
PROGRAM program
PROCEDURE procedure
TYPE type
VAR var
IF if
THEN then
ELSE else
FI fi
WHILE while
DO do
ENDWH endwh
BEGIN1 begin
END end
READ read
WRITE write
ARRAY array
OF of
RECORD record
RETURN return
INTEGER integer
CHAR char


/*符号*/
SEMI  ;
COMMA ,
ASSIGN  :=
PLUS  \+
MINUS \-
TIMES  \*
DIV   \/
DOT   \.
NOT   \!
LPAREN    \(
RPAREN    \)
LMIDPAREN    \[
RMIDPAREN    \]
UNDERANGE \.\.
RELOP >|<|>=|<=|=|!=


/*注释*/ 
COMMENT \{.*?\}
/*空白符*/
SPACE [ \f\r\t\v]+
/*换行*/
EOL \n
/*未定义字符*/
AERROR .

%%

{SPACE} {}
{COMMENT} {}
{EOL} {}
 /*关键字*/
{PROGRAM} ASTRETURN(PROGRAM);
{PROCEDURE} ASTRETURN(PROCEDURE);
{TYPE} ASTRETURN(TYPE);
{VAR} ASTRETURN(VAR);
{IF} ASTRETURN(IF);
{THEN} ASTRETURN(THEN);
{ELSE} ASTRETURN(ELSE);
{FI} ASTRETURN(FI);
{WHILE} ASTRETURN(WHILE);
{DO} ASTRETURN(DO);
{ENDWH} ASTRETURN(ENDWH);
{BEGIN1} ASTRETURN(BEGIN1);
{END} ASTRETURN(END);
{READ} ASTRETURN(READ);
{WRITE} ASTRETURN(WRITE);
{ARRAY} ASTRETURN(ARRAY);
{OF} ASTRETURN(OF);
{RECORD} ASTRETURN(RECORD);
{RETURN} ASTRETURN(RETURN);
{INTEGER} ASTRETURN(INTEGER);
{CHAR} ASTRETURN(CHAR);
 /*字面量类型表示*/
{INTC} ASTRETURN(INTC);
{CHARC} ASTRETURN(CHARC);
 /*符号*/
{SEMI} ASTRETURN(SEMI);
{COMMA} ASTRETURN(COMMA);
{ASSIGN} ASTRETURN(ASSIGN);
{PLUS} ASTRETURN(PLUS);
{MINUS} ASTRETURN(MINUS);
{TIMES} ASTRETURN(TIMES);
{DIV} ASTRETURN(DIV);
{DOT} ASTRETURN(DOT);
{NOT} ASTRETURN(NOT);
{LPAREN} ASTRETURN(LPAREN);
{RPAREN} ASTRETURN(RPAREN);
{LMIDPAREN} ASTRETURN(LMIDPAREN);
{RMIDPAREN} ASTRETURN(RMIDPAREN);

{UNDERANGE} ASTRETURN(UNDERANGE);
{RELOP} ASTRETURN(RELOP);
 /*标识符*/
{ID} ASTRETURN(ID);
 /*错误*/
{AERROR} {
	hasFault=1;
	fprintf(stderr, "Lexical Error [Line %d]: Mystirious charachter '%s'\n",yylineno,yytext);
}

%%