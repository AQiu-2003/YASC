%option yylineno
%option noyywrap

%{
	#include <stdlib.h>
	#include <stdio.h>
    #include "main.h"
    #include "parser.tab.h"
	/*生成的AST节点存储在 yylval 变量中，这是 Bison 和 Flex 共享的联合数据结构，用来传递词法和语法值。
	在 Bison 中，当识别到一个规则时，它会使用 yylval 中存储的信息来执行相应的动作代码。*/
	#define ASTRETURN(a) yylval.type_tnode=newAst(#a,0,yylineno);return a
%}

/*无符号整数*/
INTC 0|[1-9][0-9]*
/*ASCII字符*/
CHARC \'[a-zA-Z0-9@#$%^&*()_+|~`{}:"<>?/.,';\[\]\\-]\'
/*标识符*/
ID [a-zA-Z][a-zA-Z0-9]*

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
ARRAY Array
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
LPAREN    \(
RPAREN    \)
LMIDPAREN    \[
RMIDPAREN    \]
UNDERANGE \.\.
RELOP >|<|>=|<=|=|!=


/*多行{}包裹的注释*/ 
COMMENT \{[^}]*\}
/*空白符*/
SPACE [ \f\r\t\v]+
/*换行*/
EOL \n
/*未定义字符*/
AERROR .

%%

/*对于空白符、注释和换行，没有特定操作，忽略*/
{SPACE} {}
{COMMENT} {}
{EOL} {}
/*保留字、符号、标识符等，创建一个新的AST节点并返回相应的标记*/
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
{LPAREN} ASTRETURN(LPAREN);
{RPAREN} ASTRETURN(RPAREN);
{LMIDPAREN} ASTRETURN(LMIDPAREN);
{RMIDPAREN} ASTRETURN(RMIDPAREN);

{UNDERANGE} ASTRETURN(UNDERANGE);
{RELOP} ASTRETURN(RELOP);
 /*标识符*/
{ID} ASTRETURN(ID);
 /*不符合任何一定义的字符，错误处理*/
{AERROR} {
	hasFault=1;
	fprintf(stderr, "Lexical Error [Line %d]: Mystirious charachter '%s'\n",yylineno,yytext);
}

%%