%{
//#define YYSTYPE char *
#include <stdio.h>
//#include <iostream>
#include <string.h>
//#include <string>
//using namespace std;
// stuff from flex that bison needs to know about
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;

void yyerror(const char* s);
%}

%token END 0
%token PROGRAM BEGINN FUNCTION READ WRITE IF ELSE ENDIF WHILE ENDWHILE CONTINUE BREAK RETURN INT VOID STRING FLOAT
%token ASSIGN ":="
%token ADD "+"
%token MINUS "-"
%token MULTIPLY "*"
%token DIVIDE "/"
%token EQUAL "="
%token INEQUAL "!="
%token LESSTHAN "<"
%token GREATERTHAN ">"
%token LEFTPAR "("
%token RIGHTPAR ")"
%token SEMICOLON ";"
%token COMMA ','
%token LESSEQUAL "<="
%token GREATEREQUAL ">="

%union{
	int int_val;
	float float_val;
	char* str;
}
%token <int_val> INTLITERAL
%token <float_val> FLOATLITERAL
%token <str> STRINGLITERAL IDENTIFIER

%% /*Grammar rules and actions follow*/

%start program;
/*Program*/
program:	PROGRAM {
                    //printf("In program\n");
                }
		id BEGINN pgm_body END 
		;

id:		IDENTIFIER
		;

pgm_body:	decl func_declarations {
                }
		;

decl:		string_decl decl
		| var_decl decl
		| /*empty*/
		;

/* Global String Declaration */
string_decl:	STRING id ASSIGN str SEMICOLON {

                }
		;

str:		STRINGLITERAL {
                }
		;

/* Variable Declaration */
var_decl:	var_type id_list SEMICOLON
		;

var_type: 	FLOAT 
		| INT 
                
		;

any_type: 	var_type 
		| VOID 
		;

id_list: 	id id_tail {
                    //printf("In id_list $1 %s\n", $1);
                }
		;

id_tail: 	COMMA id id_tail {
                    //printf("In id_tail $2 %s\n", $2);
                } 
		| /*empty*/
		;


/* Function Paramater List */
param_decl_list:	param_decl param_decl_tail 
			| /*empty*/
			;

param_decl:		var_type id
			;

param_decl_tail:	COMMA param_decl param_decl_tail 
			| /*empty*/
			;


/* Function Declarations */
func_declarations: 	func_decl func_declarations 
			| /*empty*/
			;

func_decl: 		FUNCTION any_type id LEFTPAR param_decl_list RIGHTPAR BEGINN func_body END 
			;

func_body: 		decl stmt_list 
			;


/* Statement List */
stmt_list: 		stmt stmt_list 
			| /*empty*/
			;

stmt: 			base_stmt 
			| if_stmt 
			| while_stmt
			;

base_stmt: 		assign_stmt 
			| read_stmt 
			| write_stmt 
			| return_stmt
			;


/* Basic Statements */
assign_stmt: 		assign_expr SEMICOLON
			;

assign_expr: 		id ASSIGN expr
			;

read_stmt: 		READ LEFTPAR id_list RIGHTPAR SEMICOLON
			;

write_stmt: 		WRITE LEFTPAR id_list RIGHTPAR SEMICOLON
			;

return_stmt: 		RETURN expr SEMICOLON
			;


/* Expressions */
expr: 			expr_prefix factor
			;

expr_prefix: 		expr_prefix factor addop 
			| /*empty*/
			;

factor: 		factor_prefix postfix_expr
			;

factor_prefix: 		factor_prefix postfix_expr mulop 
			| /*empty*/
			;

postfix_expr: 		primary 
			| call_expr
			;

call_expr: 		id LEFTPAR expr_list RIGHTPAR
			;

expr_list: 		expr expr_list_tail 
			| /*empty*/
			;

expr_list_tail: 	COMMA expr expr_list_tail 
			| /*empty*/
			;

primary: 		LEFTPAR expr RIGHTPAR 
			| id 
			| INTLITERAL 
			| FLOATLITERAL
			;

addop: 			ADD
			| MINUS
			;

mulop: 			MULTIPLY
			| DIVIDE
			;


/* Complex Statements and Condition */ 
if_stmt: 		IF LEFTPAR cond RIGHTPAR decl stmt_list else_part ENDIF
			;

else_part: 		ELSE decl stmt_list 
			| /*empty*/
			;

cond: 			expr compop expr
			;

compop: 		LESSTHAN
			| GREATERTHAN 
			| EQUAL
			| INEQUAL
			| LESSEQUAL
			| GREATEREQUAL
			;


/* ECE 573 students use this version of do_while_stmt */
while_stmt: 		WHILE LEFTPAR cond RIGHTPAR decl aug_stmt_list ENDWHILE
			;

/* CONTINUE and BREAK statements. ECE 573 students only */
aug_stmt_list: 		aug_stmt aug_stmt_list 
			| /*empty*/
			;

aug_stmt: 		base_stmt 
			| aug_if_stmt 
			| while_stmt 
			| CONTINUE SEMICOLON 
			| BREAK SEMICOLON
			;


/* Augmented IF statements for ECE 573 students */ 
aug_if_stmt: 		IF LEFTPAR cond RIGHTPAR decl aug_stmt_list aug_else_part ENDIF
			;

aug_else_part: 		ELSE decl aug_stmt_list aug_else_part 
			| /*empty*/
			;

%%

int main( int argc, char** argv) {
	//if ((argc > 1) && ((yyin = freopen(argv[1], "r", stdin)) == NULL)) {
	if ((yyin = freopen(argv[1], "r", stdin)) == NULL) {
		printf("error\n");
	    //cerr << argv[0] << ": File " << argv[1] << " cannot be opened.\n";
		return 1;
	}
	  
	while (!feof(yyin))
		yyparse();

	printf("Accepted\n");
        return 0;
}

void yyerror(const char* s) { 
	printf("Not accepted\n");
	exit(1);
}
