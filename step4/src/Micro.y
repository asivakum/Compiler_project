%{
//#define YYSTYPE char *
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
//#include <string>
using namespace std;
// stuff from flex that bison needs to know about
// %token <float_val> FLOATLITERAL
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;

void yyerror(const char* s);
%}

%token END 0
%token PROGRAM BEGINN FUNCTION READ WRITE IF ELSE ENDIF WHILE ENDWHILE CONTINUE BREAK RETURN INT VOID STRING FLOAT
%token EQUAL "="
%token INEQUAL "!="
%token LESSTHAN "<"
%token GREATERTHAN ">"
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
%token <str> STRINGLITERAL IDENTIFIER FLOATLITERAL
%token <str> ASSIGN ":="
%token <str> ADD "+"
%token <str> MINUS "-"
%token <str> MULTIPLY "*"
%token <str> DIVIDE "/"
%token <str> LEFTPAR "("
%token <str> RIGHTPAR ")"

%type<str> id
%type<str> str
%type<int> INT
%type<float> FLOAT

%{
#include "../src/SymbolTableEntries.h"
//#include "../src/IRNode.h"
#include "../src/IRCodeGeneration.h"
#include "../src/TinyCodeGeneration.h"
%}
%% /*Grammar rules and actions follow*/

%start program;
/*Program*/
program:	PROGRAM {
                    //printf("In program\n");
                    CreateAndPushSymbolTableToStack("GLOBAL");
                }
		id BEGINN pgm_body END 
		;

id:		IDENTIFIER {
                    //$$ = $1;
                }
		;

pgm_body:	decl
                func_declarations 
		;

decl:		string_decl decl
		| var_decl decl
		| /*empty*/
		;

/* Global String Declaration */
string_decl:	STRING id ASSIGN str SEMICOLON {
                    //printf("\n$2 %s \n$4 %s\n", $2, $4);
                    AddStringSTEToSymbolTable($2, "STRING", $4);
                }
		;

str:		STRINGLITERAL {
                    $$ = $1;
                }
		;

/* Variable Declaration - Ashiwan changed below from var_type id_list to var_type id id_tail 
var_decl:	var_type id {
                    AddSTEToSymbolTable($<str>2, $<str>1,"null");
                } id_tail {
                    AddSTEToSymbolTable($<str>3, $<str>1,"null");
                } SEMICOLON {
                    //printf("Variable type %s and id_list %s\n",$<str>1, $<str>2);
                }
		; */
var_decl:       var_type id_list SEMICOLON {
                    //printf("Variable type %s and id_list %s\n",$<str>1, $<str>2);
                    UnknownIDTypeList($<str>2);
                    while(!temp_ids.empty()) {
                        temp_ids.top().var_type = $<str>1;
                        AddSTEToSymbolTable(temp_ids.top());
                        temp_ids.pop();
			//if (SemanticStack.top().SymbolTable.empty())
			//	printf("empty list\n");
			//else
			//	cout<<"var_name: "<<SemanticStack.top().SymbolTable.begin()->var_name<<endl;
				
                    }
		    //printf("testtest\n");
                };

var_type: 	FLOAT  {
                    $<str>$ = "FLOAT";
                } 
		| INT  {
                    $<str>$ = "INT";
                }
		;

any_type: 	var_type 
		| VOID 
		;

id_list: 	id id_tail {
                    //printf("In id $1 %s, id_tail $2 %s\n", $1, $<str>2);
                    //$<str>$ = $1;
                    $<str>$ = $<str>1;
                }
		;

id_tail: 	COMMA id id_tail {
                    //printf("Assignment with comma: id %s and id_tail %s\n",$<str>2, $<str>3); /*This might generate segmentation fault*/
                    //printf("Assignment with comma: id %s\n",$<str>2);
                    $<str>$ = $<str>2;
                    UnknownIDTypeList($<str>2);
		    //GenerateASTNode_ID($2);
                } 
		| /*empty*/
		;


/* Function Paramater List */
param_decl_list:	param_decl param_decl_tail 
			| /*empty*/
			;

param_decl:		var_type id {
                            AddStringSTEToSymbolTable($2, $<str>1, "null");
                        }
			;

param_decl_tail:	COMMA param_decl param_decl_tail 
			| /*empty*/
			;


/* Function Declarations */
func_declarations: 	func_decl func_declarations 
			| /*empty*/
			;

func_decl: 		FUNCTION any_type id {
                            CreateAndPushSymbolTableToStack($3);
                        } LEFTPAR param_decl_list RIGHTPAR BEGINN func_body END
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
assign_stmt: 		assign_expr SEMICOLON {
			    //PrintAllExprDeque();
			    GenerateIRCode_EXPR();
			}
			;

assign_expr: 		id {
			    //GenerateIRCode_ID($1);
			    PushIntoExprDeque($<str>1);
			}
			ASSIGN {
			    //GenerateASTNode_2children($2);
			    PushIntoExprDeque(":=");
			}
			expr {
			    //printf("Prepare to call GenerateIRCode_ASSIGN()\n");
			    //PrintAllSymbolTables();
			    //GenerateIRCode_ASSIGN();
			}
			;

read_stmt: 		READ LEFTPAR id_list {
                            //cout << "READ :" << $<str>3 << endl;
			    UnknownIDTypeList($<str>3);
			    while(!temp_ids.empty()) {
			        //cout << temp_ids.top().var_name << endl;
			        GenerateIRCode_READ(temp_ids.top().var_name);
				temp_ids.pop();
			    }
                        } RIGHTPAR SEMICOLON
			;

write_stmt: 		WRITE LEFTPAR id_list {
                            //cout << "WRITE:" << $<str>3 << endl;
			    UnknownIDTypeList($<str>3);
			    while(!temp_ids.empty()) {
			        //cout << temp_ids.top().var_name << endl;
			        GenerateIRCode_WRITE(temp_ids.top().var_name);
				temp_ids.pop();
			    }
			} RIGHTPAR SEMICOLON
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

primary: 		LEFTPAR {PushIntoExprDeque("(");} expr RIGHTPAR {PushIntoExprDeque(")");}
			| id {
			      //GenerateASTNode_ID($4);
			      //GenerateIRCode_ID($1);
			      PushIntoExprDeque($1);
			  }
			| INTLITERAL {
			      PushIntoExprDeque(to_string((long long int)$1));
			      //GenerateASTNode_INTLITERAL($5);
			      //printf("int literal: %d\n", $1);
			      //GenerateIRCode_INTLITERAL(to_string((long long int)$1));
			  }
			| FLOATLITERAL {
			      PushIntoExprDeque($1);
			      //GenerateIRCode_FLOATLITERAL(to_string((long double)$1));
			      //GenerateIRCode_FLOATLITERAL($1);
		 	  }
			;

addop: 			ADD {
			      PushIntoExprDeque("+");
			      //GenerateIRCode_addop();
			}
			| MINUS {
			      PushIntoExprDeque("-");
			      //GenerateIRCode_addop();
			}	
			;

mulop: 			MULTIPLY {
			      PushIntoExprDeque("*");
			}
			| DIVIDE {
			      PushIntoExprDeque("/");
			      //GenerateIRCode_mulop();
			}
			;


/* Complex Statements and Condition */ 
if_stmt: 		IF{
                            std::ostringstream oss;
                            block_cnt += 1;
                            oss << block_cnt;
                            string my_scope_name;
                            my_scope_name.append(blk_scope_name);
                            my_scope_name += oss.str();
                            CreateAndPushSymbolTableToStack(my_scope_name);
                        } LEFTPAR cond RIGHTPAR decl stmt_list else_part ENDIF 
			;

else_part: 		ELSE{
                            std::ostringstream oss;
                            block_cnt += 1;
                            oss << block_cnt;
                            string my_scope_name;
                            my_scope_name.append(blk_scope_name);
                            my_scope_name += oss.str();
                            CreateAndPushSymbolTableToStack(my_scope_name);
                        } decl stmt_list  
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
while_stmt: 		WHILE {
                            std::ostringstream oss;
                            block_cnt += 1;
                            oss << block_cnt;
                            string my_scope_name;
                            my_scope_name.append(blk_scope_name);
                            my_scope_name += oss.str();
                            CreateAndPushSymbolTableToStack(my_scope_name);
                        } LEFTPAR cond RIGHTPAR decl aug_stmt_list ENDWHILE
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
aug_if_stmt: 		IF {
                            std::ostringstream oss;
                            block_cnt += 1;
                            oss << block_cnt;
                            string my_scope_name;
                            my_scope_name.append(blk_scope_name);
                            my_scope_name += oss.str();
                            CreateAndPushSymbolTableToStack(my_scope_name);
                        } LEFTPAR cond RIGHTPAR decl aug_stmt_list aug_else_part ENDIF
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
	//printf("Accepted\n");
        if(CheckDeclError()) {
            return 0;
        }
        CheckShadowWarning();
        //PrintAllSymbolTables();
        PrintIRList();
        GenerateTinyCodeFromIR();
        PrintTinyCode();
	return 0;
}

void yyerror(const char* s) { 
	printf("Not accepted\n");
	exit(1);
}
