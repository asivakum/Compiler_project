/* scanner for a Micro programming language */

/*"\""[a-zA-Z0-9\t\n ./<>?;:'`!@#$%^&*()[]{}_+=|\\-]*"\""  {*/

/*.          printf( "Unrecognized character: %s\n", yytext );*/
/*(.)+          {
            printf( "Illegal token: %s\n", yytext );
            exit(1);
	    }
ILLEGALTOKEN	[^({DIGIT}|{IDENTIFIER}|{FLOATLITERAL}|{KEYWORD}|{STRINGLITERAL}|{OPERATOR}|{COMMENT}|{WHITESYMBOL})]+

{KEYWORD}       {
	    yylval.keyword = strdup(yytext); return KEYWORD;
            printf("Token Type: KEYWORD\n");
            printf("Value: %s\n", yytext);
            }
{OPERATOR}  {
            yylval.operator_ = strdup(yytext); return OPERATOR;
            printf("Token Type: OPERATOR\n");
            printf("Value: %s\n", yytext);
            }
*/
%{
#include <math.h>
#include <errno.h>
#include "Micro.tab.h"
#define YY_DECL extern "C" int yylex()
%}

DIGIT    [0-9]
IDENTIFIER       [a-zA-Z][a-zA-Z0-9]*
FLOATLITERAL	{DIGIT}+"."{DIGIT}*|"."{DIGIT}*
KEYWORD		PROGRAM|BEGIN|END|FUNCTION|READ|WRITE|IF|ELSE|ENDIF|WHILE|ENDWHILE|CONTINUE|BREAK|RETURN|INT|VOID|STRING|FLOAT
STRINGLITERAL	\"([^"])*\"
OPERATOR	":="|"+"|"-"|"*"|"/"|"="|"!="|"<"|">"|"("|")"|";"|","|"<="|">="
COMMENT		--([^\n])*\n
WHITESYMBOL	[ \t\n]+
ILLEGALTOKEN	.

%%
{DIGIT}+    {
            yylval.int_val = atoi(yytext); return INTLITERAL;
            printf("Token Type: INTLITERAL\n");
            printf("Value: %s\n", yytext);
            }

{FLOATLITERAL}        {
            yylval.float_val = atof(yytext); return FLOATLITERAL;
            printf("Token Type: FLOATLITERAL\n");
            printf("Value: %s\n", yytext);
            }


"PROGRAM" {return PROGRAM;}
"BEGIN" {return BEGINN;}
"END" {return END;}
"FUNCTION" {return FUNCTION;}
"READ" {return READ;}
"WRITE" {return WRITE;}
"IF" {return IF;}
"ELSE" {return ELSE;}
"ENDIF" {return ENDIF;}
"WHILE" {return WHILE;}
"ENDWHILE" {return ENDWHILE;}
"CONTINUE" {return CONTINUE;}
"BREAK" {return BREAK;}
"RETURN" {return RETURN;}
"INT" {return INT;}
"VOID" {return VOID;}
"STRING" {return STRING;}
"FLOAT" {return FLOAT;}


{STRINGLITERAL}  {
            yylval.str = strdup(yytext); return STRINGLITERAL;
            printf("Token Type: STRINGLITERAL\n");
            printf("Value: %s\n", yytext);
            }

{IDENTIFIER}  {
            yylval.str = strdup(yytext); return IDENTIFIER;
	    printf("Token Type: IDENTIFIER\n");
            printf("Value: %s\n", yytext);
            }

":=" {return ASSIGN;}
"+" {return ADD;}
"-" {return MINUS;}
"*" {return MULTIPLY;}
"/" {return DIVIDE;}
"=" {return EQUAL;}
"!=" {return INEQUAL;}
"<" {return LESSTHAN;}
">" {return GREATERTHAN;}
"(" {return LEFTPAR;}
")" {return RIGHTPAR;}
";" {return SEMICOLON;}
"," {return COMMA;}
"<=" {return LESSEQUAL;}
">=" {return GREATEREQUAL;}


{COMMENT} {}

{WHITESYMBOL} {}

{ILLEGALTOKEN} {
            printf( "Illegal token: %s\n", yytext );
            exit(1);
	    }

%%

//int main( int argc, char **argv )
//    {
//    ++argv, --argc;  /* skip over program name */
//    if ( argc > 0 )
//            {
//            yyin = fopen( argv[0], "r" );
//            if (yyin == NULL) 
//                {
//	      	perror("Error printed by perror");
//                exit(1);
//                }
//            }
//    else
//            yyin = stdin;
//
//    yylex();
//    }

