#include <cstdlib>
#include <stack>
#include <string>
#include <list>
#include <deque>
#include <sstream>
#include "SymbolTableEntries.h"
#include "IRNode.h"

using namespace std;

//typedef struct threeAddrCode {
//    string opcode;
//    string operand_1;
//    string operand_2;
//    string result;
//} IRNode;
//
//list<IRNode> IRList;

deque<string> expr;
stack<string> temp_expr_to_hold;
deque<string> temp_expr_to_combine;

void PushIntoExprDeque(string symbol) {
    expr.push_back(symbol);
}

string SplitStringByComma(string &input) {
    string delimiter = ",";
    size_t pos = 0;
    string token;
    pos = input.find(delimiter);
    if(pos == string::npos) {
        token = input;
        input.clear();
        return token;
    } else {
        token = input.substr(0, pos);
        input.erase(0, pos + delimiter.length());
        return token;
    }
}


int isValue(string input) {
   istringstream iss1(input);
   istringstream iss2(input);
   float float_temp;
   int int_temp;
   iss1 >> noskipws >> int_temp;
   if(iss1 && iss1.eof())
       return 1;
   iss2 >> noskipws >> float_temp;
   if(iss2 && iss2.eof())
       return 2;
   return 0; 
}


void GenerateIRCodeFromPostOrderExpr() {
    string my_str = expr.back();
    while(!my_str.empty()) {
        string token = SplitStringByComma(my_str);
        //cout << token << endl;
        if(token == "*" | token == "/") {
            GenerateIRCode_mulop(token);
        }
        else if(token == "+" | token == "-") {
            GenerateIRCode_addop(token);
        }
        else if(token == ":=") {
            GenerateIRCode_ASSIGN();
        }
        else if(token == ",") {
           //cout << "Ignoring comma" << endl; 
        }
        else if(isValue(token) == 1) {
           GenerateIRCode_VAL(token, "INT");
        }
        else if(isValue(token) == 2) {
           GenerateIRCode_VAL(token, "FLOAT");
        }
        else {
           GenerateIRCode_ID(token);
        }
    }
}

void CombineExprWithoutParenthesis(deque<string> &expr) {
    deque<string>::iterator iter;
    deque<string>::iterator temp_iter;
    string postOrderSegment;

    for (iter = expr.begin(); iter != expr.end(); iter++) {
    	if (*iter == "*" | *iter == "/") {
	    //temp_iter = iter;
	    postOrderSegment = *(iter - 1) + "," + *(iter + 1) + "," + *iter;
	    *(iter - 1) = postOrderSegment;
	    temp_iter = iter - 1;
	    expr.erase(iter, iter + 2);
	    iter = temp_iter;
	}
    }

    for (iter = expr.begin(); iter != expr.end(); iter++) {
    	if (*iter == "+" | *iter == "-") {
	    //temp_iter = iter;
	    //postOrderSegment = *(iter - 1) + *(iter + 1) + *iter;
	    postOrderSegment = *(iter - 1) + "," + *(iter + 1) + "," + *iter;
	    *(iter - 1) = postOrderSegment;
	    temp_iter = iter - 1;
	    expr.erase(iter, iter + 2);
	    iter = temp_iter;
	}
    }

    for (iter = expr.begin(); iter != expr.end(); iter++) {
    	if (*iter == ":=") {
	    //postOrderSegment = *(iter - 1) + *(iter + 1) + *iter;
	    postOrderSegment = *(iter - 1) + "," + *(iter + 1) + "," + *iter;
	    *(iter - 1) = postOrderSegment;
	    temp_iter = iter - 1;
	    expr.erase(iter, iter + 2);
	    iter = temp_iter;
	}
    }
}

void GenerateIRCode_EXPR() {
    /* Deal with () */
    while (!expr.empty()) {
    	if (expr.back() != "(") {
	    temp_expr_to_hold.push(expr.back());
	    expr.pop_back();
	}
	else if (expr.back() == "("){
	    //temp_expr_to_combine.push_back(*expr.end());
	    expr.pop_back(); // Removed the last '('
	    while (temp_expr_to_hold.top() != ")") {
	    	temp_expr_to_combine.push_back( temp_expr_to_hold.top() );
		temp_expr_to_hold.pop();
	    }
	    /* Removed the first ')' */
	    //temp_expr_to_combine.push_back( temp_expr_to_hold.top() );
	    temp_expr_to_hold.pop();

	    CombineExprWithoutParenthesis(temp_expr_to_combine);
	    /*After this function, the temp_expr_to_combine should contain only 1 string*/

	    expr.push_back( temp_expr_to_combine.back() );

	    /* Put symbols back to the expr deque */
	    while (!temp_expr_to_hold.empty()) {
	    	expr.push_back( temp_expr_to_hold.top() );
		temp_expr_to_hold.pop();
	    }
	}
    }
    /* No () from here on, everything now in the temp_expr_to_hold stack */
    while (!temp_expr_to_hold.empty()) {
	expr.push_back( temp_expr_to_hold.top() );
	temp_expr_to_hold.pop();
    }

    CombineExprWithoutParenthesis(expr);

    //cout << expr.back() << endl;
    GenerateIRCodeFromPostOrderExpr();
    expr.pop_back();

}


void PrintAllExprDeque() {
    deque<string>::iterator iter;
    for (iter = expr.begin(); iter != expr.end(); iter++) {
    	cout << *iter;
    }
    cout << endl;
}

