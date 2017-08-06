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
deque<string> temp_for_params;
deque<string> temp_expr_to_combine;
stack<string> temp_push_params_to_hold;
IRNode ret_temp;

void CreateLabelAndLinkInIRCode(string funcName) {
    IRNode temp1, temp2;
    temp1.opcode = "LABEL";
    temp1.result = funcName;
    IRList.push_back(temp1);
    temp2.opcode = "LINK";
    IRList.push_back(temp2);
    FuncIRList[currFuncName].push_back(temp1);
    FuncIRList[currFuncName].push_back(temp2);
}

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


void GenerateFromPostOrder_ForParams() {
    string my_str = temp_for_params.back();
    while(!my_str.empty()) {
        string token = SplitStringByComma(my_str);
        //string token = temp_expr_to_hold.top();
        //cout << "Params: " << token << endl;
        if(token == "*" | token == "/") {
            ret_temp = GenerateIRCode_mulop(token);
        }
        else if(token == "+" | token == "-") {
            ret_temp = GenerateIRCode_addop(token);
        }
        else if(token == ":=") {
            ret_temp = GenerateIRCode_ASSIGN();
        }
        else if(token == ",") {
           //cout << "Ignoring comma" << endl; 
        }
        else if(isValue(token) == 1) {
           ret_temp = GenerateIRCode_VAL(token, "INT");
        }
        else if(isValue(token) == 2) {
           ret_temp = GenerateIRCode_VAL(token, "FLOAT");
        }
        else {
           GenerateIRCode_ID(token);
        }
        //temp_expr_to_hold.pop();
    }
}



void GenerateIRCodeFromPostOrderExpr() {
    string my_str = expr.back();
    while(!my_str.empty()) {
        string token = SplitStringByComma(my_str);
        //cout << token << endl;
        if(token == "*" | token == "/") {
            ret_temp = GenerateIRCode_mulop(token);
        }
        else if(token == "+" | token == "-") {
            ret_temp = GenerateIRCode_addop(token);
        }
        else if(token == ":=") {
            ret_temp = GenerateIRCode_ASSIGN();
        }
        else if(token == ",") {
           //cout << "Ignoring comma" << endl; 
        }
        else if(isValue(token) == 1) {
           ret_temp = GenerateIRCode_VAL(token, "INT");
        }
        else if(isValue(token) == 2) {
           ret_temp = GenerateIRCode_VAL(token, "FLOAT");
        }
        else {
           GenerateIRCode_ID(token);
        }
    }
}

void GenerateIRCodeFromIFPostOrderExpr() {
    string my_str = expr.back();
    while(!my_str.empty()) {
        string token = SplitStringByComma(my_str);
        //cout << token << endl;
        if(token == "*" | token == "/") {
            ret_temp = GenerateIRCode_mulop(token);
        }
        else if(token == "+" | token == "-") {
            ret_temp = GenerateIRCode_addop(token);
        }
        else if(token == "<" | token == "<=" | token == ">" | token == ">=" | token == "==" | token == "!=") {
            GenerateIRCode_COMP(token);
        }
        else if(token == ",") {
           //cout << "Ignoring comma" << endl; 
        }
        else if(isValue(token) == 1) {
           ret_temp = GenerateIRCode_VAL(token, "INT");
        }
        else if(isValue(token) == 2) {
           ret_temp = GenerateIRCode_VAL(token, "FLOAT");
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
    int myflg = 0;

    for (iter = expr.begin(); iter != expr.end(); iter++) {
    	if (*iter == "*" | *iter == "/") {
	    //temp_iter = iter;
	    postOrderSegment = *(iter - 1) + "," + *(iter + 1) + "," + *iter;
	    *(iter - 1) = postOrderSegment;
	    temp_iter = iter - 1;
	    expr.erase(iter, iter + 2);
	    iter = temp_iter;
            myflg = 1;
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
            myflg = 1;
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
            myflg = 1;
	}
    }
    // Ashiwan modified for step5 to take care of comparator
    for (iter = expr.begin(); iter != expr.end(); iter++) {
    	if (*iter == "<=" | *iter == "<" | *iter == ">=" | *iter == ">" | *iter == "==" | *iter == "!=") {
	    //temp_iter = iter;
	    //postOrderSegment = *(iter - 1) + *(iter + 1) + *iter;
	    postOrderSegment = *(iter - 1) + "," + *(iter + 1) + "," + *iter;
	    *(iter - 1) = postOrderSegment;
	    temp_iter = iter - 1;
	    expr.erase(iter, iter + 2);
	    iter = temp_iter;
            myflg = 1;
	}
    }

    // Ashiwan modified for step6 for call expr
    //if (myflg == 0 && expr.size()>1) {
    //    for (iter = expr.begin(); iter != expr.end(); iter++) {
    ////        cout << "Val1 " << *iter << "Val2 " << *(iter+1) << endl;
    //        //if (*iter == ",") 
    //        postOrderSegment = *(iter + 1) + "," + *iter;
    //        *(iter) = postOrderSegment;
    //        temp_iter = iter;
    //        expr.erase(iter+1);
    //        iter = temp_iter;
    //    }
    //}
}

void GenerateIRCode_EXPR() {
    /* Deal with () */
    while (!expr.empty()) {
        //cout << "Expr: " << expr.back() << endl;
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
         //   cout << "Temp expr to combine: " << temp_expr_to_combine.back() << endl;
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
 
    //deque<string>::iterator iter;
    //cout << "Expr Before: ";
    //for (iter = expr.begin(); iter != expr.end(); iter++)
    //    cout << *iter << " ";
    //cout << endl;

    CombineExprWithoutParenthesis(expr);

    //cout << "Expr After: " << expr.back() << endl;
    GenerateIRCodeFromPostOrderExpr();
    expr.pop_back();

}

void GenerateIRCode_CondExpr() {
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

    //cout << "Done with one condition" << endl;
    //cout << expr.back() << endl;
    //cout << "Done printing one condition" << endl;
    GenerateIRCodeFromIFPostOrderExpr();
    expr.pop_back();

}

void GenerateIRCode_Param_CallEXPR() {

    while (!expr.empty()) {
       //cout << "Call expr Expr: " << expr.back() << endl;
        if (expr.back() == ":=") {
            break;
        }
    	else if (expr.back() != "(") {
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
        //    cout << "Call expr Temp expr to combine: " << temp_expr_to_combine.back() << endl;

	    /* Put symbols back to the expr deque */
	    while (!temp_expr_to_hold.empty()) {
		temp_expr_to_hold.pop();
	    }
	}
    }
 /// Ashiwan step6 - no paranthesized expression in function calls issue
    /* No () from here on, everything now in the temp_expr_to_hold stack */
    //cout << "Call expr temp expr to hold: ";
    //cout << temp_expr_to_hold.top() << endl;
    //cout << "Call expr size of : " << temp_expr_to_hold.size() << endl;
    if(temp_expr_to_hold.size() == 1) {
       string IR_name = GetIRVarName(temp_expr_to_hold.top());
       if(IR_name == "\0")
           IR_name = temp_expr_to_hold.top(); 
       temp_push_params_to_hold.push(IR_name);
       temp_expr_to_hold.pop();
    } else {
      while(!temp_expr_to_hold.empty()) {
         temp_for_params.push_back(temp_expr_to_hold.top());
         temp_expr_to_hold.pop();
      }
      // Call GenerateFromPostOrder and get the result
       CombineExprWithoutParenthesis(temp_for_params);
       GenerateFromPostOrder_ForParams();
       string push_ir_name = FuncIRList[currFuncName].back().result;
       temp_push_params_to_hold.push(push_ir_name);
    }
}

void GenerateIRCode_CallEXPR(string func) {
    IRNode temp;
    temp.opcode = "PUSH";
    temp.operand_1 = ""; 
    temp.operand_2 = "";
    temp.result = "";
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
    //cout << "Push vals : ";
    int numParams = 0;
    while(!temp_push_params_to_hold.empty()) {
     //   cout << temp_push_params_to_hold.top() << " ";
        temp.result = temp_push_params_to_hold.top();
        IRList.push_back(temp);
        FuncIRList[currFuncName].push_back(temp);
        temp_push_params_to_hold.pop();
        numParams++;
    }
    //cout << endl;
    temp.opcode = "JSR";
    temp.result = func;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
    temp.opcode = "POP";
    temp.result = "";
    for(int jj=0; jj < numParams; jj++) {
        IRList.push_back(temp);
        FuncIRList[currFuncName].push_back(temp);
    } 
    string result_temp = "$T" + to_string(FuncScope[currFuncName].tempVarCount++);
    temp.result = result_temp;
    FuncScope[currFuncName].tempVarCount++;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
    expr.push_back(result_temp);
    string ret_type = FuncScope[func].ret_type;
    
    STE myste;
    myste.var_name = result_temp;
    myste.var_type = ret_type;
    myste.value = "";
    myste.IR_var_name = result_temp;
    FuncScope[currFuncName].SymbolTable.push_back(myste);
}


void GenerateIRCode_RET() {
    
    //Check if expr is only 0 or 1 or variable or expr?
//    cout << "expr_size " << expr.size() << endl;
    GenerateIRCode_EXPR();
    IRNode temp1, temp2;
    //iter = FuncIRList[currFuncName].back();
    string end_result = ret_temp.result;
    string end_opcode = ret_temp.opcode;
    //cout << "End opcode: " << end_opcode << "End result: " << end_result << endl;
    if(end_opcode == "STOREI" || end_opcode == "ADDI" || end_opcode == "SUBI" || end_opcode == "MULTI" || end_opcode == "DIVI")
        temp1.opcode = "STOREI";
    else if(end_opcode == "STOREF" || end_opcode == "ADDF" || end_opcode == "SUBF" || end_opcode == "MULTF" || end_opcode == "DIVF")
        temp1.opcode = "STOREF";
        
    temp1.operand_1 = end_result;
    temp1.operand_2 = "\0";
    temp1.result = "$T"+to_string(FuncScope[currFuncName].tempVarCount);
    FuncScope[currFuncName].tempVarCount++;
    IRList.push_back(temp1);
    FuncIRList[currFuncName].push_back(temp1);
    temp1.operand_1 = temp1.result;
    temp1.result = "$R";
    IRList.push_back(temp1);
    FuncIRList[currFuncName].push_back(temp1);
    
    temp2.opcode = "RET";
    temp2.operand_1 = "";
    temp2.operand_2 = "";
    temp2.result = "";
    IRList.push_back(temp2);
    FuncIRList[currFuncName].push_back(temp2);
}


void PrintAllExprDeque() {
    deque<string>::iterator iter;
    for (iter = expr.begin(); iter != expr.end(); iter++) {
    	cout << *iter;
    }
    cout << endl;
}

