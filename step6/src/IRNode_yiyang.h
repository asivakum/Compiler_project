#include <cstdlib>
#include <stack>
#include <string>
#include <list>
#include "SymbolTableEntries.h"

using namespace std;

long long int tempVarCount = 1;

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

void GenerateIRCode_ID(string id) {
    //cout << "GenerateIRCode_ID" << endl;
    string id_type;
    id_type = SearchSymbolTableForType(id);
    if (id_type.empty()) {
    	cerr << "[ERROR] ID doesn't exist in the symbol table!" << endl;
	abort();
    }
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
    else
    	cerr << "[ERROR] Should never fall here!" << endl;

    temp.operand_1 = rightChild.name;
    temp.operand_2 = "\0";
    temp.result = leftChild.name;
    IRList.push_back(temp);
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


//void GenerateIRCode_addop() {
//    tempASTNode leftChild, rightChild;
//    IRNode temp;
//
//    rightChild = tempStackForIRCode.top();
//    tempStackForIRCode.pop();
//    leftChild = tempStackForIRCode.top();
//    tempStackForIRCode.pop();
//
//    if (leftChild.type == "INT" && rightChild.type == "INT")
//	temp.opcode = "ADDI";
//    else if (leftChild.type == "FLOAT" && rightChild.type == "FLOAT") 
//	temp.opcode = "ADDF";
//    else
//    	cerr << "[ERROR] Should never fall here!" << endl;
//
//    temp.operand_1 = leftChild.name;
//    temp.operand_2 = rightChild.name;
//    temp.result = "$T"+to_string(tempVarCount);
//    tempVarCount++;
//    IRList.push_back(temp);
//
//    tempStackForIRCode.push(ASTNode(temp.result, leftChild.type));
//}


void GenerateIRCode_mulop() {
}

void PrintIRList() {
    list<IRNode>::iterator iter;
    for (iter = IRList.begin(); iter != IRList.end(); iter++) {
    	cout << ";" << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << " " << iter->result << endl;
    }
}
