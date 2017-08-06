/* scanner for a Micro programming language */

/*"\""[a-zA-Z0-9\t\n ./<>?;:'`!@#$%^&*()[]{}_+=|\\-]*"\""  {*/

/*.          printf( "Unrecognized character: %s\n", yytext );*/
/*(.)+          {
            printf( "Illegal token: %s\n", yytext );
            exit(1);
	    }
ILLEGALTOKEN	[^({DIGIT}|{IDENTIFIER}|{FLOATLITERAL}|{KEYWORD}|{STRINGLITERAL}|{OPERATOR}|{COMMENT}|{WHITESYMBOL})]+
*/
%{
#include <math.h>
#include <errno.h>
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
            printf("Token Type: INTLITERAL\n");
            printf("Value: %s\n", yytext);
            }

{FLOATLITERAL}        {
            printf("Token Type: FLOATLITERAL\n");
            printf("Value: %s\n", yytext);
            }

{KEYWORD}       {
            printf("Token Type: KEYWORD\n");
            printf("Value: %s\n", yytext);
            }
{STRINGLITERAL}  {
            printf("Token Type: STRINGLITERAL\n");
            printf("Value: %s\n", yytext);
            }

{IDENTIFIER}        {
                     printf("Token Type: IDENTIFIER\n");
                     printf("Value: %s\n", yytext);
                    }

{OPERATOR}   {
             printf("Token Type: OPERATOR\n");
             printf("Value: %s\n", yytext);
             }

{COMMENT} {}

{WHITESYMBOL} {}

{ILLEGALTOKEN} {
            printf( "Illegal token: %s\n", yytext );
            exit(1);
	    }

%%

int main( int argc, char **argv )
    {
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
            {
            yyin = fopen( argv[0], "r" );
            if (yyin == NULL) 
                {
	      	perror("Error printed by perror");
                exit(1);
                }
            }
    else
            yyin = stdin;

    yylex();
    }

