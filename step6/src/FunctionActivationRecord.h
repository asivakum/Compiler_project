#include<iostream>
#include<string>
#include<stack>
#include<list>
#include<map>
#include"SymbolTableEntries.h"
#include"IRNode.h"

using namespace std;


typedef struct FunctionSpecificData {
    string funcName;
    int numLocalVar;
    int numParams;
    long long int tempVarCount = 1;
    Scope funcSymbolTable;
    list<IRNode> funcIRList;
} FuncData;

//Global hash map for each function to hold the function's symbol table
//and the function specific IRCode
map<string, FuncData> FuncMap; 


//void CreateFunctionObject(string id) {
//	if (id == "main") {
//		Function *f = new Function(id);
//	}
//	else {
//		Function *f = new Function(id, allFunctionList.top());
//	}
//	allFunctionList.push_back(f);
//
//	f->GenerateIRCode_FUNCTION_ID();
//	f->GenerateIRCode_FUNCTION_LINK();
//}
//
//void StoreParam(string id, string type) {
//	if (!allFunctionList.empty()) {
//		Function *f = allFunctionList.back();
//	}
//	else {
//		cerr << "[ERROR] allFunctionList is empty!" << endl;
//	}
//	f->InsertParam(id, type);
//}
//
//void Function::GenerateIRCode_FUNCTION_ID() {
//	IRNode temp;
//	temp.opcode = "LABEL";
//	temp.operand_1 = "";
//	temp.operand_2 = "";
//	temp.result = this->name;
//	this->IRList.push_back(temp);
//}
//
//void Function::GenerateIRCode_FUNCTION_LINK() {
//	IRNode temp;
//	temp.opcode = "LINK";
//	temp.operand_1 = "";
//	temp.operand_2 = "";
//	temp.result = "";
//	this->IRList.push_back(temp);
//}
//
//void Function::InsertParam(string &id, string &type) {
//	vector<string> temp;
//	temp.push_back(type);
//	temp.push_back("");
//	this->symbolTable[id] = temp;
//}
