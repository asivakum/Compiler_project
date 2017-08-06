#include <cstdlib>
#include <stack>
#include <string>
#include <list>
#include "SymbolTableEntries.h"

using namespace std;

//long long int tempVarCount = 1;

typedef struct threeAddrCode {
    string opcode;
    string operand_1;
    string operand_2;
    string result;
} IRNode;

typedef struct ASTNode {
    string name;
    string type;
    ASTNode(string s1, string s2) {
    	name = s1;
	type = s2;
    }
    ASTNode() {}
} tempASTNode;

stack<tempASTNode> tempStackForIRCode;
list<IRNode> IRList;
map<string, list<IRNode>> FuncIRList;
map<string, string> ret_expr_variable;

void PrintFuncIRList(string func) {
    //cout << ";IR code" << endl;
    list<IRNode>::iterator iter;
    for (iter = FuncIRList[func].begin(); iter != FuncIRList[func].end(); iter++) {
    	//cout << ";" << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << " " << iter->result << endl;
	cout << ";" << iter->opcode;
	if (iter->operand_1 != "")
		cout << " " << iter->operand_1;
	if (iter->operand_2 != "")
		cout << " " << iter->operand_2;
	if (iter->result != "")
		cout << " " << iter->result;
        //if(!(iter->opcode == "PUSH" && iter->result == ""))
	  cout << endl;
    }
    /*if(func != "global") {
        if(FuncIRList[func].back().opcode != "RET")
            cout << ";RET" << endl;
    }*/
}



void GenerateIRCode_ID(string id) {
    //cout << "GenerateIRCode_ID" << endl;
    string id_type;
    if(id.find("$T") != std::string::npos) {
        id_type = SearchCurrFuncSymbolTableForType(id);
        //cout << id << " " << id_type << endl;
        tempStackForIRCode.push(ASTNode(id, id_type));
        return;
    }
    id_type = SearchSymbolTableForType(id);
    //PrintAllSymbolTables();
    if (id_type.empty()) {
    	cerr << "[ERROR] In ID : ID doesn't exist in the symbol table!" << endl;
	abort();
    }
    //if(currFuncName == "main")
    //cout << id << " " << id_type << endl;
    tempStackForIRCode.push(ASTNode(id, id_type));
}

IRNode GenerateIRCode_INTLITERAL(string literal) {
    //cout << "GenerateIRCode_INTLITERAL" << endl;
    IRNode temp;
//    cout << "intliteral calling getirvarname" << endl;
    string IR_literal = GetIRVarName(literal);
    if(IR_literal == "\0") {
    	//cerr << "[ERROR] IR var name not available!" << endl;
	//abort();
	IR_literal = literal;
    }
    temp.opcode = "STOREI";
    //temp.operand_1 = literal;
    temp.operand_1 = IR_literal;
    temp.operand_2 = "\0";
    //Ashiwan step7 made tempcounts to be local to a func
    temp.result = "$T"+to_string(FuncScope[currFuncName].tempVarCount);
    FuncScope[currFuncName].tempVarCount++;
    //Ashiwan added in step6 to separate out function specific IR Code
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
    tempStackForIRCode.push(ASTNode(temp.result, "INT"));
    return temp;
}

IRNode GenerateIRCode_FLOATLITERAL(string literal) {
    IRNode temp;
//    cout << "floatliteral calling getirvarname" << endl;
    string IR_literal = GetIRVarName(literal);
    if(IR_literal == "\0") {
    	//cerr << "[ERROR] IR var name not available!" << endl;
	//abort();
	IR_literal = literal;
    }
    temp.opcode = "STOREF";
    //temp.operand_1 = literal;
    temp.operand_1 = IR_literal;
    temp.operand_2 = "\0";
    temp.result = "$T"+to_string(FuncScope[currFuncName].tempVarCount);
    FuncScope[currFuncName].tempVarCount++;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);

    tempStackForIRCode.push(ASTNode(temp.result, "FLOAT"));
    return temp;
}

IRNode GenerateIRCode_ASSIGN() {
    tempASTNode leftChild, rightChild;
    IRNode temp;

    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();

    //id_type = SearchSymbolTableForType(leftChild);
    //if (leftChild.type == "INT" && rightChild.type == "INT")
    if (leftChild.type == "INT")
        temp.opcode = "STOREI";
    //else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") 
    else if (leftChild.type == "FLOAT") 
        temp.opcode = "STOREF";
    else {
    	cerr << "1. [ERROR] Should never fall here!" << endl;
        abort();
    }

//    cout << "assign calling getirvarname" << endl;
    string IR_literal_left = GetIRVarName(leftChild.name);
    string IR_literal_right = GetIRVarName(rightChild.name);
    if(IR_literal_left == "\0" || IR_literal_right == "\0") {
    	cerr << "[ERROR] IR var name not available!" << endl;
	abort();
    }
    //temp.operand_1 = rightChild.name;
    temp.operand_1 = IR_literal_right;
    temp.operand_2 = "\0";
    //temp.result = leftChild.name;
    temp.result = IR_literal_left;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
    return temp;
}

IRNode GenerateIRCode_VAL(string tok, string val_type) {
    IRNode temp;
    if (val_type == "INT") {
        temp = GenerateIRCode_INTLITERAL(tok);
        //cout << "In val : " << temp.opcode << " " << temp.operand_1 << " " << temp.operand_2 << " " << temp.result << endl;
    }
    else if (val_type == "FLOAT") {
        temp = GenerateIRCode_FLOATLITERAL(tok);	
        //cout << "In val : " << temp.opcode << " " << temp.operand_1 << " " << temp.operand_2 << " " << temp.result << endl;
    }
    else
    	cerr << "2. [ERROR] Should never fall here!!" << endl;
    return temp;
}


IRNode GenerateIRCode_addop(string oper) {
    tempASTNode leftChild, rightChild;
    IRNode temp;

    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    string result_type;

    if (leftChild.type == "INT" && rightChild.type == "INT") {
        if(oper == "+") 
	    temp.opcode = "ADDI";
        else
	    temp.opcode = "SUBI";
        result_type = "INT";
    }
    //else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") { 
    else if (leftChild.type == "FLOAT" || rightChild.type == "FLOAT") { 
        if(oper == "+") 
	    temp.opcode = "ADDF";
        else
	    temp.opcode = "SUBF";
        result_type = "FLOAT";
    }
    else
    	cerr << "3. [ERROR] Should never fall here!!!" << endl;
//    cout << "addop calling getirvarname" << endl;
    string IR_literal_left = GetIRVarName(leftChild.name);
    string IR_literal_right = GetIRVarName(rightChild.name);
    if(IR_literal_left == "\0" || IR_literal_right == "\0") {
    	cerr << "[ERROR] IR var name not available!" << endl;
	abort();
    }
    //temp.operand_1 = rightChild.name;
    temp.operand_1 = IR_literal_left;
    temp.operand_2 = IR_literal_right;
    //temp.operand_2 = leftChild.name;
    temp.result = "$T"+to_string(FuncScope[currFuncName].tempVarCount);
    FuncScope[currFuncName].tempVarCount++;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
    tempStackForIRCode.push(ASTNode(temp.result, result_type));
    return temp;
}

IRNode GenerateIRCode_mulop(string oper) {
    tempASTNode leftChild, rightChild;
    IRNode temp;

    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    string result_type;

    if (leftChild.type == "INT" && rightChild.type == "INT") {
        if(oper == "*") 
	    temp.opcode = "MULTI";
        else
	    temp.opcode = "DIVI";
        result_type = "INT";
    }
    //else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") { 
    else if (leftChild.type == "FLOAT" || rightChild.type == "FLOAT") { 
        if(oper == "*") 
	    temp.opcode = "MULTF";
        else
	    temp.opcode = "DIVF";
        result_type = "FLOAT";
    }
    else
    	cerr << "4. [ERROR] Should never fall here!!!!" << endl;
    string IR_literal_left = GetIRVarName(leftChild.name);
    string IR_literal_right = GetIRVarName(rightChild.name);
    if(IR_literal_left == "\0" || IR_literal_right == "\0") {
    	cerr << "[ERROR] IR var name not available!" << endl;
        abort();
    }
    //temp.operand_1 = rightChild.name;
    temp.operand_1 = IR_literal_left;
    temp.operand_2 = IR_literal_right;
    //temp.operand_2 = leftChild.name;
    temp.result = "$T"+to_string(FuncScope[currFuncName].tempVarCount);
    FuncScope[currFuncName].tempVarCount++;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);

    tempStackForIRCode.push(ASTNode(temp.result, result_type));
    return temp;
}

void GenerateIRCode_COMP(string cmpop) {
    tempASTNode leftChild, rightChild;
    IRNode temp;
    string temp_code;
    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    string result_type;
    //id_type = SearchSymbolTableForType(leftChild);
    if(cmpop == "<")
       temp_code = "GE"; 
    else if(cmpop == "<=")
       temp_code = "GT"; 
    else if(cmpop == ">")
       temp_code = "LE"; 
    else if(cmpop == ">=")
       temp_code = "LT"; 
    else if(cmpop == "==")
       temp_code = "NE";
    else if(cmpop == "!=")
       temp_code = "EQ";

    if (leftChild.type == "INT" && rightChild.type == "INT") {
        temp.opcode = temp_code + "I"; 
        result_type = "INT";
    }
    //else if ((leftChild.type == "FLOAT" && rightChild.type == "FLOAT")) 
    else if ((leftChild.type == "FLOAT" || rightChild.type == "FLOAT")) {
        temp.opcode = temp_code + "F";
        result_type = "FLOAT";
    }
    else {
        //cout << leftChild.type << endl;
        //cout << rightChild.type << endl;
        //cout << "Current func " << currFuncName << endl;
    	//cerr << "5. [ERROR] Should never fall here!" << endl;
        //abort();
        temp.opcode = temp_code + "I";
        result_type = "INT";
    }

    string IR_literal_left = GetIRVarName(leftChild.name);
    string IR_literal_right = GetIRVarName(rightChild.name);
    if(IR_literal_left == "\0" || IR_literal_right == "\0") {
    	cerr << "[ERROR] IR var name not available!" << endl;
	abort();
    }
    //temp.operand_1 = rightChild.name;
    temp.operand_1 = IR_literal_left;
    temp.operand_2 = IR_literal_right;
    //temp.operand_2 = leftChild.name;
    vector<string> my_lbls = ScopeStack.top().labels;
    temp.result = my_lbls[0];
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
}

void GenerateIRCode_READ(string id) {
    string id_type;
    IRNode temp;
    id_type = SearchSymbolTableForType(id);
    if (id_type.empty()) {
    	cerr << "[ERROR] ID doesn't exist in the symbol table!" << endl;
        abort();
    }
    if (id_type == "INT") {
        temp.opcode = "READI";
    }
    else if (id_type == "FLOAT") {
    	temp.opcode = "READF";
    }
    else if (id_type == "STRING") {
    	temp.opcode = "READS";
    }
    else {
    	cerr << "6. [ERROR] Should never fall here!" << endl;
        abort();
    }

    string IR_id = GetIRVarName(id);
    if(IR_id == "\0") {
    	cerr << "[ERROR] IR var name not available!" << endl;
	abort();
    }
    //temp.result = id;
    temp.result = IR_id;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);

}

void GenerateIRCode_WRITE(string id) {
    string id_type;
    IRNode temp;
    //cout << "In write : " << id << endl;
    id_type = SearchSymbolTableForType(id);
    if (id_type.empty()) {
    	cerr << "[ERROR] ID doesn't exist in the symbol table!" << endl;
	abort();
    }
    if (id_type == "INT") {
        temp.opcode = "WRITEI";
    }
    else if (id_type == "FLOAT") {
    	temp.opcode = "WRITEF";
    }
    else if (id_type == "STRING") {
    	temp.opcode = "WRITES";
    }
    else {
    	cerr << "7. [ERROR] Should never fall here!" << endl;
        abort();
    }

    string IR_id = GetIRVarName(id);
    if(IR_id == "\0") {
    	cerr << "[ERROR] IR var name not available!" << endl;
        abort();
    }
    //temp.result = id;
    temp.result = IR_id;
    IRList.push_back(temp);
    FuncIRList[currFuncName].push_back(temp);
}

void GenerateIRCode_JMPENDIF() {
 // Function to jump to ENDIF after done with stmts inside IF
 // Function will be called only if ELSE block is available. 
 // Called inside ELSE block's scope. pop twice and get the label and psh it back
   Scope scope_temp = ScopeStack.top();
   ScopeStack.pop();
   string jmp_lbl = ScopeStack.top().labels[1];
   ScopeStack.push(scope_temp);
   IRNode temp;
   temp.opcode = "JUMP";
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = jmp_lbl;
   IRList.push_back(temp);
   FuncIRList[currFuncName].push_back(temp);
}

void GenerateIRCode_AddLblENDIF() {
 // Function to add ENDIF label 
 // Done with IF, ELSE, ENDIF
 // So popping ELSE scope, then get label name from IF Scope and pop it out too
 // Taking label 0 or 1 depending on whether there is an ELSE block or not
   string jmp_lbl;
   if(ScopeStack.top().blk_type == "IF") {
       jmp_lbl = ScopeStack.top().labels[0];
       ScopeStack.pop();
   }
   else if(ScopeStack.top().blk_type == "ELSE") {
       ScopeStack.pop();
       jmp_lbl = ScopeStack.top().labels[1];
       ScopeStack.pop();
   }
   IRNode temp;
   temp.opcode = "LABEL"; 
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = jmp_lbl;
   IRList.push_back(temp);
   FuncIRList[currFuncName].push_back(temp);
}


void GenerateIRCode_AddELSELBL() {
   Scope scope_temp = ScopeStack.top();
   ScopeStack.pop();
   string else_lbl = ScopeStack.top().labels[0];
   ScopeStack.push(scope_temp);
   IRNode temp;
   temp.opcode = "LABEL";
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = else_lbl;
   IRList.push_back(temp);
   FuncIRList[currFuncName].push_back(temp);
}

void GenerateIRCode_AddWHILELBL() {
   // For while loops label 0 is end and label 1 is beginning of while
   string while_lbl = ScopeStack.top().labels[1];
   IRNode temp;
   temp.opcode = "LABEL"; 
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = while_lbl;
   IRList.push_back(temp);
   FuncIRList[currFuncName].push_back(temp);
}

void GenerateIRCode_while_loop_and_add_endwhile() {
   // For while loops label 0 is end and label 1 is beginning of while
   string while_lbl = ScopeStack.top().labels[1];
   IRNode temp1;
   IRNode temp2;
   temp1.opcode = "JUMP";
   temp1.operand_1 = "";
   temp1.operand_2 = "";
   temp1.result = while_lbl;
   IRList.push_back(temp1);
   FuncIRList[currFuncName].push_back(temp1);
   string end_while_lbl = ScopeStack.top().labels[0];
   temp2.opcode = "LABEL"; 
   temp2.operand_1 = "";
   temp2.operand_2 = "";
   temp2.result = end_while_lbl;
   IRList.push_back(temp2);
   FuncIRList[currFuncName].push_back(temp2);
   ScopeStack.pop();
}

void GenerateIRCode_BREAK() {
    stack<Scope> temp_semantic_stack;
    while (ScopeStack.top().blk_type != "WHILE") {
    	temp_semantic_stack.push(ScopeStack.top());
	ScopeStack.pop();
    }
    string end_while_lbl = ScopeStack.top().labels[0];
    IRNode temp;
    temp.opcode = "JUMP";
    temp.operand_1 = "";
    temp.operand_2 = "";
    temp.result = end_while_lbl;
    IRList.push_back(temp);
   FuncIRList[currFuncName].push_back(temp);
    while (!temp_semantic_stack.empty()) {
    	ScopeStack.push(temp_semantic_stack.top());
	temp_semantic_stack.pop();
    }
}

void GenerateIRCode_CONTINUE() {
    stack<Scope> temp_semantic_stack;
    while (ScopeStack.top().blk_type != "WHILE") {
    	temp_semantic_stack.push(ScopeStack.top());
	ScopeStack.pop();
    }
    string while_lbl = ScopeStack.top().labels[1];
    IRNode temp;
    temp.opcode = "JUMP";
    temp.operand_1 = "";
    temp.operand_2 = "";
    temp.result = while_lbl;
    IRList.push_back(temp);
   FuncIRList[currFuncName].push_back(temp);
    while (!temp_semantic_stack.empty()) {
    	ScopeStack.push(temp_semantic_stack.top());
	temp_semantic_stack.pop();
    }
}



void PrintIRList() {
    cout << ";IR code" << endl;
    list<string>::const_iterator iter;
    
    for (iter = FuncNames.begin(); iter != FuncNames.end(); iter++) {
        PrintFuncIRList(*iter); 
    }


    //list<IRNode>::iterator iter;
    //for (iter = IRList.begin(); iter != IRList.end(); iter++) {
    //	//cout << ";" << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << " " << iter->result << endl;
    //    cout << ";" << iter->opcode;
    //    if (iter->operand_1 != "")
    //    	cout << " " << iter->operand_1;
    //    if (iter->operand_2 != "")
    //    	cout << " " << iter->operand_2;
    //    if (iter->result != "")
    //    	cout << " " << iter->result;
    //    cout << endl;
    //}
}


