#include <cstdlib>
#include <stack>
#include <string>
#include <list>
#include <deque>
#include <sstream>
#include "SymbolTableEntries.h"

using namespace std;

long long int registerCount = 0;
long long int tot_temp_vars = 0;
long long int tot_local_vars = 0;
string currFunc = "";

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

void PushAllRegistersToStack() {
    TinyNode temp;
    temp.opcode = "push";
    for (long long int kk=0; kk < tot_temp_vars; kk++) {
        temp.operand_1 = "r" + to_string(kk);
        temp.operand_2 = "";
        TinyList.push_back(temp);
    }
}

void PopAllRegistersFromStack() {
    TinyNode temp;
    temp.opcode = "pop";
    for (long long int kk=tot_temp_vars-1; kk > 0; kk--) {
        temp.operand_1 = "r" + to_string(kk);
        temp.operand_2 = "";
        TinyList.push_back(temp);
    }
}

void VariableDeclFromSymbolTable() {
    list<STE>::iterator iter;
    stack <Scope> tempStack;
    //tempStack = ScopeStack;
    TinyNode temp;
    //PrintAllSymbolTables();
//    while(!tempStack.empty()) {
        //Scope currScope = tempStack.top();
        iter = globalScope.SymbolTable.begin();
        while(iter != globalScope.SymbolTable.end()) {
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
     //Ashiwan step6 count the total temp variables
     
    list<string>::const_iterator tt;
    for (tt = FuncNames.begin(); tt != FuncNames.end(); tt++) {
         tot_temp_vars += (FuncScope[*tt].tempVarCount - 1);
         //cout << "Temp var cnt : " << tot_temp_vars << endl;
    }
    temp.opcode = "push";
    temp.operand_1 = "";
    temp.operand_2 = "";
    TinyList.push_back(temp);
    PushAllRegistersToStack();
    temp.opcode = "jsr";
    temp.operand_1 = "main";
    TinyList.push_back(temp);
    temp.opcode = "sys";
    temp.operand_1 = "halt";
    TinyList.push_back(temp);
    //    tempStack.pop();
   // }
}

void IRCodeVar2TinyCodeStack(string &reg, const string &operand) {

    // reg[0] = 'r';
    if (operand[0] == '$') {
        if(operand[1] == 'L') {
            long long int tempCount;
            reg = "$-";
    	    istringstream(operand.substr(2, string::npos)) >> tempCount;
            reg += to_string(tempCount);
        }

        if(operand[1] == 'P') {
            long long int tempCount;
            reg = "$";
    	    istringstream(operand.substr(2, string::npos)) >> tempCount;
            tempCount = tempCount + tot_temp_vars + 1;
            reg += to_string(tempCount);
        }

        if(operand[1] == 'T') {
            long long int tempCount;
            reg = "r";
    	    istringstream(operand.substr(2, string::npos)) >> tempCount;
    	    tempCount--;
    	    reg += to_string(tempCount);
            registerCount = tempCount;
        }

        if(operand[1] == 'R') {
            long long int tempCount;
            reg = "$";
            //cout << "tiny : curr func name " << currFunc << endl;
            //cout << "tiny : param count " << (FuncScope[currFunc].ParamCount -1) << endl;
            // ParamCount already has a +1
            tempCount = (tot_temp_vars + FuncScope[currFunc].ParamCount);
            reg += to_string(tempCount);
        }
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("addi", reg2, reg_result) );
	}
    	else if (iter->opcode == "ADDF" ) {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("addr", reg2, reg_result) );
	}
    	else if (iter->opcode == "SUBI" ) {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("subi", reg2, reg_result) );
	}
    	else if (iter->opcode == "SUBF" ) {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("subr", reg2, reg_result) );
	}
    	else if (iter->opcode == "MULTI" ) {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("muli", reg2, reg_result) );
	}
    	else if (iter->opcode == "MULTF" ) {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("mulr", reg2, reg_result) );
	}
    	else if (iter->opcode == "DIVI" ) {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("divi", reg2, reg_result) );
	}
    	else if (iter->opcode == "DIVF" ) {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg2, iter->operand_2);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    
	    TinyList.push_back( TinyCode("move", reg1, reg_result) );
	    TinyList.push_back( TinyCode("divr", reg2, reg_result) );
	}
        // Ashiwan temporary dirty hack for step5 to fix a = b type code.
        // Not reusing registers. Just using a new register from global count
    	else if (iter->opcode == "STOREI" || iter->opcode == "STOREF") {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
            if(reg1[0]!='r' && reg_result[0]!='r') {
                string t_reg = 'r' + to_string(++registerCount);
	        TinyList.push_back( TinyCode("move", reg1, t_reg ) );
	        TinyList.push_back( TinyCode("move", t_reg, reg_result ) );
            } else {
	        TinyList.push_back( TinyCode("move", reg1, reg_result) );
            }
	}
    	else if (iter->opcode == "GTI") {
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg1, iter->operand_1);
	    IRCodeVar2TinyCodeStack(reg_result, iter->operand_2);
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
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("sys", "readi", reg_result) );
	}
    	else if (iter->opcode == "READF" ) {
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("sys", "readr", reg_result) );
	}
    	else if (iter->opcode == "READS" ) {
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("sys", "reads", reg_result) );
	}
    	else if (iter->opcode == "WRITEI" ) {
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("sys", "writei", reg_result) );
	}
    	else if (iter->opcode == "WRITEF" ) {
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("sys", "writer", reg_result) );
	}
    	else if (iter->opcode == "WRITES" ) {
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("sys", "writes", reg_result) );
	}
    	else if (iter->opcode == "JUMP" ) {
	    TinyList.push_back( TinyCode("jmp", "", iter->result) );
	}
    	else if (iter->opcode == "LABEL" ) {
	    TinyList.push_back( TinyCode("label", "", iter->result) );
            if(iter->result.find("label") == std::string::npos)
                currFunc = iter->result;
	}
        else if (iter->opcode == "PUSH") {
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("push", "", reg_result));
        }
        else if (iter->opcode == "POP") {
	    IRCodeVar2TinyCodeStack(reg_result, iter->result);
	    TinyList.push_back( TinyCode("pop", "", reg_result));
        }
        else if (iter->opcode == "LINK") {
            //cout << "tiny : curr func name " << currFunc << endl;
            //cout << "tiny : local var count " << (FuncScope[currFunc].LocalVarCount) << endl;
	    TinyList.push_back( TinyCode("link", "", to_string(FuncScope[currFunc].LocalVarCount)));
        }
        else if (iter->opcode == "JSR") {
            PushAllRegistersToStack();
	    TinyList.push_back( TinyCode("jsr", "", iter->result));
            PopAllRegistersFromStack();
        }
        else if (iter->opcode == "RET") {
	    TinyList.push_back( TinyCode("unlnk", "", ""));
	    TinyList.push_back( TinyCode("ret", "", ""));
        }
    }
}

void PrintTinyCode() {
    cout << ";tiny code" << endl;
    list<TinyNode>::iterator iter;
    for (iter = TinyList.begin(); iter != TinyList.end(); iter++) {
        if(iter->opcode == "str")  {
    	    cout << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << endl;
        }
        else if(iter->opcode == "var") {
    	    cout << iter->opcode << " " << iter->operand_1 << endl;
        }
        else {
            if(iter->operand_1 == "" && iter->operand_2  == "")
    	        cout << iter->opcode << endl; 
            else if(iter->operand_1 == "")
    	        cout << iter->opcode << " " << iter->operand_2 << endl; 
            else if(iter->operand_2 == "")
    	        cout << iter->opcode << " " << iter->operand_1 << endl; 
            else
    	        cout << iter->opcode << " " << iter->operand_1 << " " << iter->operand_2 << endl;
        }
    }
//    cout << "sys halt" << endl;
}
