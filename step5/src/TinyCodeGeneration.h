#include <cstdlib>
#include <stack>
#include <string>
#include <list>
#include <deque>
#include <sstream>
#include "SymbolTableEntries.h"

using namespace std;

long long int registerCount = 0;

typedef struct TinyCode {
    string opcode;
    string operand_1;
    string operand_2;
    TinyCode(string s0, string s1, string s2) {
    	opcode = s0;
	operand_1 = s1;
	operand_2 = s2;
    }
    TinyCode() {}
} TinyNode;

list<TinyNode> TinyList;

void VariableDeclFromSymbolTable() {
    list<STE>::iterator iter;
    stack <Scope> tempStack;
    tempStack = SemanticStack;
    TinyNode temp;
    //PrintAllSymbolTables();
    while(!tempStack.empty()) {
        Scope currScope = tempStack.top();
        iter = currScope.SymbolTable.begin();
        while(iter != currScope.SymbolTable.end()) {
            if(iter->var_type == "STRING") {
                temp.opcode = "str";
                temp.operand_1 = iter->var_name;
                temp.operand_2 = iter->value;
            } else {
                temp.opcode = "var";
                temp.operand_1 = iter->var_name;
            }
            TinyList.push_back(temp);
            iter++;
        }
        tempStack.pop();
    }
}

void IRCodeTempVar2TinyCodeReg(string &reg, const string &operand) {
    
    long long int tempCount;
    // reg[0] = 'r';
    if (operand[0] == '$') {
        reg = "r";
    	istringstream(operand.substr(2, string::npos)) >> tempCount;
    	tempCount--;
    	reg += to_string(tempCount);
        registerCount = tempCount;
    }
    else {
    	reg = operand;
    }
}

void GenerateTinyCodeFromIR() {
    VariableDeclFromSymbolTable();

    list<IRNode>::iterator iter;
    //TinyNode temp;
    string reg1, reg2, reg_result;
    for (iter = IRList.begin(); iter != IRList.end(); iter++) {
    	if (iter->opcode == "ADDI" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("addi", reg2, reg_result) );
	}
    	else if (iter->opcode == "ADDF" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("addr", reg2, reg_result) );
	}
    	else if (iter->opcode == "SUBI" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("subi", reg2, reg_result) );
	}
    	else if (iter->opcode == "SUBF" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("subr", reg2, reg_result) );
	}
    	else if (iter->opcode == "MULTI" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("muli", reg2, reg_result) );
	}
    	else if (iter->opcode == "MULTF" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("mulr", reg2, reg_result) );
	}
    	else if (iter->opcode == "DIVI" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("divi", reg2, reg_result) );
	}
    	else if (iter->opcode == "DIVF" ) {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg2, iter->operand_2);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("divr", reg2, reg_result) );
	}
        // Ashiwan temporary dirty hack for step5 to fix a = b type code.
        // Not reusing registers. Just using a new register from global count
    	else if (iter->opcode == "STOREI" || iter->opcode == "STOREF") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->result);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg1, t_reg ) );
	        TinyList.push_back( TinyCode("move", t_reg, reg_result ) );
            } else {
	        TinyList.push_back( TinyCode("move", reg1, reg_result) );
            }
	}
    	else if (iter->opcode == "GTI") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpi", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jgt", "", iter->result) );
	}
    	else if (iter->opcode == "GTF") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpr", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jgt", "", iter->result) );
	}
    	else if (iter->opcode == "GEI") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpi", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jge", "", iter->result) );
	}
    	else if (iter->opcode == "GEF") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpr", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jge", "", iter->result) );
	}
    	else if (iter->opcode == "LTI") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpi", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jlt", "", iter->result) );
	}
    	else if (iter->opcode == "LTF") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpr", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jlt", "", iter->result) );
	}
    	else if (iter->opcode == "LEI") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpi", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jle", "", iter->result) );
	}
    	else if (iter->opcode == "LEF") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpr", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jle", "", iter->result) );
	}
    	else if (iter->opcode == "EQI") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpi", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jeq", "", iter->result) );
	}
    	else if (iter->opcode == "EQF") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpr", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jeq", "", iter->result) );
	}
    	else if (iter->opcode == "NEI") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpi", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jne", "", iter->result) );
	}
    	else if (iter->opcode == "NEF") {
	    IRCodeTempVar2TinyCodeReg(reg1, iter->operand_1);
	    IRCodeTempVar2TinyCodeReg(reg_result, iter->operand_2);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg_result, t_reg ) );
	        TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
            } else {
	        TinyList.push_back( TinyCode("cmpr", reg1, reg_result) );
            }
	    TinyList.push_back( TinyCode("jne", "", iter->result) );
	}
    	else if (iter->opcode == "READI" ) {
	    TinyList.push_back( TinyCode("sys", "readi", iter->result) );
	}
    	else if (iter->opcode == "READF" ) {
	    TinyList.push_back( TinyCode("sys", "readr", iter->result) );
	}
    	else if (iter->opcode == "READS" ) {
	    TinyList.push_back( TinyCode("sys", "reads", iter->result) );
	}
    	else if (iter->opcode == "WRITEI" ) {
	    TinyList.push_back( TinyCode("sys", "writei", iter->result) );
	}
    	else if (iter->opcode == "WRITEF" ) {
	    TinyList.push_back( TinyCode("sys", "writer", iter->result) );
	}
    	else if (iter->opcode == "WRITES" ) {
	    TinyList.push_back( TinyCode("sys", "writes", iter->result) );
	}
    	else if (iter->opcode == "JUMP" ) {
	    TinyList.push_back( TinyCode("jmp", "", iter->result) );
	}
    	else if (iter->opcode == "LABEL" ) {
	    TinyList.push_back( TinyCode("label", "", iter->result) );
	}
    }
}

void PrintTinyCode() {
    cout << ";tiny code" << endl;
    list<TinyNode>::iterator iter;
    for (iter = TinyList.begin(); iter != TinyList.end(); iter++) {
        if(iter->opcode == "str") 
    	    cout << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << endl;
        else if(iter->opcode == "var")
    	    cout << iter->opcode << " " << iter->operand_1 << endl;
        else
    	    cout << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << endl;
    }
    cout << "sys halt" << endl;
}
