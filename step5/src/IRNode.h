#include <cstdlib>
#include <stack>
#include <string>
#include <list>
#include "SymbolTableEntries.h"

using namespace std;

long long int tempVarCount = 1;
long long int labelCount = 1;

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

string SearchSymbolTableForType(string id) {
    /* Search current Scope */
    list<STE>::iterator iter;
    iter = SemanticStack.top().SymbolTable.begin();
    //cout<<"id:"<<id<<endl;
    //if (SemanticStack.top().SymbolTable.empty())
    //	cout<<"empty"<<endl;
    //cout<<"stack: "<<iter->var_name<<endl;
    while(iter != SemanticStack.top().SymbolTable.end()) {
    	//cout<<"stack: "<<iter->var_name<<endl;
    	if (iter->var_name == id) {
	    return iter->var_type;
	}
    	iter++;
    }
    iter = globalScope.SymbolTable.begin();
    while(iter != globalScope.SymbolTable.end()) {
    	//cout<<"stack: "<<iter->var_name<<endl;
    	if (iter->var_name == id) {
	    return iter->var_type;
	}
    	iter++;
    }
    /* Search global Scope */

    return "\0";
}


vector<string> SearchSymbolTableForLabel(string my_blk_type) {
    if(my_blk_type == "IF" || my_blk_type == "WHILE")
        return SemanticStack.top().labels;
    else if(my_blk_type == "ELSE") {
        Scope temp = SemanticStack.top();
        SemanticStack.pop();
        vector<string> labels = SemanticStack.top().labels;
        SemanticStack.push(temp);
        return labels;
     }
}

void GenerateIRCode_ID(string id) {
    //cout << "GenerateIRCode_ID" << endl;
    string id_type;
    id_type = SearchSymbolTableForType(id);
    //PrintAllSymbolTables();
    if (id_type.empty()) {
    	cerr << "[ERROR] ID doesn't exist in the symbol table!" << endl;
	abort();
    }
    //cout << id << " " << id_type << endl;
    tempStackForIRCode.push(ASTNode(id, id_type));
}

void GenerateIRCode_INTLITERAL(string literal) {
    //cout << "GenerateIRCode_INTLITERAL" << endl;
    IRNode temp;
    temp.opcode = "STOREI";
    temp.operand_1 = literal;
    temp.operand_2 = "\0";
    temp.result = "$T"+to_string(tempVarCount);
    tempVarCount++;
    IRList.push_back(temp);

    tempStackForIRCode.push(ASTNode(temp.result, "INT"));
}

void GenerateIRCode_FLOATLITERAL(string literal) {
    IRNode temp;
    temp.opcode = "STOREF";
    temp.operand_1 = literal;
    temp.operand_2 = "\0";
    temp.result = "$T"+to_string(tempVarCount);
    tempVarCount++;
    IRList.push_back(temp);

    tempStackForIRCode.push(ASTNode(temp.result, "FLOAT"));
}

void GenerateIRCode_ASSIGN() {
    tempASTNode leftChild, rightChild;
    IRNode temp;

    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();

    //id_type = SearchSymbolTableForType(leftChild);
    if (leftChild.type == "INT" && rightChild.type == "INT")
	temp.opcode = "STOREI";
    else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") 
	temp.opcode = "STOREF";
    else {
    	cerr << "[ERROR] Should never fall here!" << endl;
	abort();
    }
    temp.operand_1 = rightChild.name;
    temp.operand_2 = "\0";
    temp.result = leftChild.name;
    IRList.push_back(temp);
}

void GenerateIRCode_VAL(string tok, string val_type) {
    if (val_type == "INT")
        GenerateIRCode_INTLITERAL(tok);	
    else if (val_type == "FLOAT") 
        GenerateIRCode_FLOATLITERAL(tok);	
    else
    	cerr << "[ERROR] Should never fall here!" << endl;
}


void GenerateIRCode_addop(string oper) {
    tempASTNode leftChild, rightChild;
    IRNode temp;

    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();

    if (leftChild.type == "INT" && rightChild.type == "INT") {
        if(oper == "+") 
	    temp.opcode = "ADDI";
        else
	    temp.opcode = "SUBI";
    }
    else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") { 
        if(oper == "+") 
	    temp.opcode = "ADDF";
        else
	    temp.opcode = "SUBF";
    }
    else
    	cerr << "[ERROR] Should never fall here!" << endl;

    temp.operand_1 = leftChild.name;
    temp.operand_2 = rightChild.name;
    temp.result = "$T"+to_string(tempVarCount);
    tempVarCount++;
    IRList.push_back(temp);
    tempStackForIRCode.push(ASTNode(temp.result, leftChild.type));
}

void GenerateIRCode_mulop(string oper) {
    tempASTNode leftChild, rightChild;
    IRNode temp;

    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();

    if (leftChild.type == "INT" && rightChild.type == "INT") {
        if(oper == "*") 
	    temp.opcode = "MULTI";
        else
	    temp.opcode = "DIVI";
    }
    else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") { 
        if(oper == "*") 
	    temp.opcode = "MULTF";
        else
	    temp.opcode = "DIVF";
    }
    else
    	cerr << "[ERROR] Should never fall here!" << endl;

    temp.operand_1 = leftChild.name;
    temp.operand_2 = rightChild.name;
    temp.result = "$T"+to_string(tempVarCount);
    tempVarCount++;
    IRList.push_back(temp);

    tempStackForIRCode.push(ASTNode(temp.result, leftChild.type));
}

void GenerateIRCode_COMP(string cmpop) {
    tempASTNode leftChild, rightChild;
    IRNode temp;
    string temp_code;
    rightChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();
    leftChild = tempStackForIRCode.top();
    tempStackForIRCode.pop();

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
    if (leftChild.type == "INT" && rightChild.type == "INT")
	temp.opcode = temp_code + "I"; 
    else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") 
	temp.opcode = temp_code + "F";
    else {
    	cerr << "[ERROR] Should never fall here!" << endl;
	abort();
    }
    temp.operand_1 = leftChild.name;
    temp.operand_2 = rightChild.name;
    vector<string> my_lbls = SemanticStack.top().labels;
    temp.result = my_lbls[0];
    IRList.push_back(temp);
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
    	cerr << "[ERROR] Should never fall here!" << endl;
	abort();
    }
    temp.result = id;
    IRList.push_back(temp);

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
    	cerr << "[ERROR] Should never fall here!" << endl;
	abort();
    }
    temp.result = id;
    IRList.push_back(temp);
}

void GenerateIRCode_JMPENDIF() {
 // Function to jump to ENDIF after done with stmts inside IF
 // Function will be called only if ELSE block is available. 
 // Called inside ELSE block's scope. pop twice and get the label and psh it back
   Scope scope_temp = SemanticStack.top();
   SemanticStack.pop();
   string jmp_lbl = SemanticStack.top().labels[1];
   SemanticStack.push(scope_temp);
   IRNode temp;
   temp.opcode = "JUMP";
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = jmp_lbl;
   IRList.push_back(temp);
}

void GenerateIRCode_AddLblENDIF() {
 // Function to add ENDIF label 
 // Done with IF, ELSE, ENDIF
 // So popping ELSE scope, then get label name from IF Scope and pop it out too
 // Taking label 0 or 1 depending on whether there is an ELSE block or not
   string jmp_lbl;
   if(SemanticStack.top().blk_type == "IF") {
       jmp_lbl = SemanticStack.top().labels[0];
       SemanticStack.pop();
   }
   else if(SemanticStack.top().blk_type == "ELSE") {
       SemanticStack.pop();
       jmp_lbl = SemanticStack.top().labels[1];
       SemanticStack.pop();
   }
   IRNode temp;
   temp.opcode = "LABEL"; 
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = jmp_lbl;
   IRList.push_back(temp);
}


void GenerateIRCode_AddELSELBL() {
   Scope scope_temp = SemanticStack.top();
   SemanticStack.pop();
   string else_lbl = SemanticStack.top().labels[0];
   SemanticStack.push(scope_temp);
   IRNode temp;
   temp.opcode = "LABEL";
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = else_lbl;
   IRList.push_back(temp);
}

void GenerateIRCode_AddWHILELBL() {
   // For while loops label 0 is end and label 1 is beginning of while
   string while_lbl = SemanticStack.top().labels[1];
   IRNode temp;
   temp.opcode = "LABEL"; 
   temp.operand_1 = "";
   temp.operand_2 = "";
   temp.result = while_lbl;
   IRList.push_back(temp);
}

void GenerateIRCode_while_loop_and_add_endwhile() {
   // For while loops label 0 is end and label 1 is beginning of while
   string while_lbl = SemanticStack.top().labels[1];
   IRNode temp1;
   IRNode temp2;
   temp1.opcode = "JUMP";
   temp1.operand_1 = "";
   temp1.operand_2 = "";
   temp1.result = while_lbl;
   IRList.push_back(temp1);
   string end_while_lbl = SemanticStack.top().labels[0];
   temp2.opcode = "LABEL"; 
   temp1.operand_1 = "";
   temp1.operand_2 = "";
   temp2.result = end_while_lbl;
   IRList.push_back(temp2);
   SemanticStack.pop();
}

void GenerateIRCode_BREAK() {
    stack<Scope> temp_semantic_stack;
    while (SemanticStack.top().blk_type != "WHILE") {
    	temp_semantic_stack.push(SemanticStack.top());
	SemanticStack.pop();
    }
    string end_while_lbl = SemanticStack.top().labels[0];
    IRNode temp;
    temp.opcode = "JUMP";
    temp.operand_1 = "";
    temp.operand_2 = "";
    temp.result = end_while_lbl;
    IRList.push_back(temp);
    while (!temp_semantic_stack.empty()) {
    	SemanticStack.push(temp_semantic_stack.top());
	temp_semantic_stack.pop();
    }
}

void GenerateIRCode_CONTINUE() {
    stack<Scope> temp_semantic_stack;
    while (SemanticStack.top().blk_type != "WHILE") {
    	temp_semantic_stack.push(SemanticStack.top());
	SemanticStack.pop();
    }
    string while_lbl = SemanticStack.top().labels[1];
    IRNode temp;
    temp.opcode = "JUMP";
    temp.operand_1 = "";
    temp.operand_2 = "";
    temp.result = while_lbl;
    IRList.push_back(temp);
    while (!temp_semantic_stack.empty()) {
    	SemanticStack.push(temp_semantic_stack.top());
	temp_semantic_stack.pop();
    }
}


void PrintIRList() {
    cout << ";IR code" << endl;
    list<IRNode>::iterator iter;
    for (iter = IRList.begin(); iter != IRList.end(); iter++) {
    	//cout << ";" << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << " " << iter->result << endl;
	cout << ";" << iter->opcode;
	if (iter->operand_1 != "")
		cout << " " << iter->operand_1;
	if (iter->operand_2 != "")
		cout << " " << iter->operand_2;
	if (iter->result != "")
		cout << " " << iter->result;
	cout << endl;
    }
}
