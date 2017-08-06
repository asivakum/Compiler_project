#include <cstdlib>
#include <stack>
#include <string>
#include <list>
#include <deque>
#include <sstream>
#include "SymbolTableEntries.h"
#define NUMBER_REG 4
//#include "ControlFlowGraphGeneration.h"

using namespace std;
list<ControlFlowGraphNode>::const_iterator it_statement;

long long int registerCount = 0;
long long int tot_temp_vars = 0;
long long int tot_local_vars = 0;
string currFunc = "";

class Register {
public:
    string value;
    bool isDirty;
    bool isBanned; // flag to mark if it is being used currently
    bool isFree;
    Register() {value = ""; isDirty = false; isBanned = false; isFree = false;}
};

Register r[NUMBER_REG];

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
    //for (long long int kk=0; kk < tot_temp_vars; kk++) {
    //    temp.operand_1 = "r" + to_string(kk);
    //    temp.operand_2 = "";
    //    TinyList.push_back(temp);
    //}

    // modified by Yiyang for step7
    for (long long int i = 0; i < NUMBER_REG; i++) {
        temp.operand_1 = "r" + to_string(i);
        temp.operand_2 = "";
        TinyList.push_back(temp);
    }
}

void PopAllRegistersFromStack() {
    TinyNode temp;
    temp.opcode = "pop";
    for (long long int i = NUMBER_REG - 1; i >= 0; i--) {
        temp.operand_1 = "r" + to_string(i);
        temp.operand_2 = "";
        TinyList.push_back(temp);
    }
    //for (long long int kk=tot_temp_vars-1; kk > 0; kk--) {
    //    temp.operand_1 = "r" + to_string(kk);
    //    temp.operand_2 = "";
    //    TinyList.push_back(temp);
    //}
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

bool isNumber(string opr) {
    //cout << "in isNumber(): opr = " << opr << endl;
    if (opr[0] == '0' || opr[0] == '1' || opr[0] == '2' || opr[0] == '3' || opr[0] == '4' || opr[0] == '5'  || opr[0] == '6' || opr[0] == '7' || opr[0] == '8' || opr[0] == '9' || opr[0] == '.') 
        return true;
    return false;
}

void RA_free(long long int id) {
    string spill_dst;
    list<ControlFlowGraphNode>::const_iterator it = it_statement; // have to use a global variable to pass the iterator... so annoying to pass via functions

    if (r[id].isDirty && it -> Live_OUT.find(r[id].value) != it -> Live_OUT.end()) { /*is dirty and live*/
        /* spill, generate store */
        //FuncScope[currFunc].SpillCount++;
        spill_dst = r[id].value;
        //if (r[id].value[0] != '$' && !isNumber(r[id].value))
        //    spill_dst = r[id].value;
        //else if (r[id].value[0] == '$' )
        //    spill_dst = "$-" + to_string(FuncScope[currFunc].tempVarCount + FuncScope[currFunc].SpillCount);
        TinyList.push_back( TinyCode("move", "r"+to_string(id), spill_dst) );
    }
    r[id].isFree = true;
    //r[id].value = "";
    //r[id].isDirty = false;
}

long long int RA_allocate(string opr) {
    long long int regId = -1;
    //cout << "r0: " <<  r[0].value << endl;
    //cout << "r1: " <<  r[1].value << endl;
    //cout << "r2: " <<  r[2].value << endl;
    //cout << "r3: " <<  r[3].value << endl;
    //cout << "regId = " << regId << endl;
    //cout << "start of RA_allocate" << endl;
    for (long long int i = 0; i < NUMBER_REG; i++) {
        //cout << "i = " << i << endl;
        if (r[i].value.empty()) {
            regId = i;
            break;
        }
    }
    //cout << "r0: " <<  r[0].value << endl;
    //cout << "r1: " <<  r[1].value << endl;
    //cout << "r2: " <<  r[2].value << endl;
    //cout << "r3: " <<  r[3].value << endl;
    //cout << "regId = " << regId << endl;
    if (regId == -1){
        //try to find a non-dirty register 
        for (long long int i = 0; i < NUMBER_REG; i++) {
            if (!r[i].isDirty) {
                regId = i;
            }
            break;
        }
        // if can not, use the 1st one not banned
        if (regId == -1) {
            for (long long int i = 0; i < NUMBER_REG; i++) {
                if (!r[i].isBanned) {
                    regId = i;
                }
                break;
            }
            if (regId == -1) //This should never happend, but just in case
                regId = 3;
        }
        RA_free(regId);
    }
    //cout << "r0: " <<  r[0].value << endl;
    //cout << "r1: " <<  r[1].value << endl;
    //cout << "r2: " <<  r[2].value << endl;
    //cout << "r3: " <<  r[3].value << endl;
    //cout << "regId = " << regId << endl;
    r[regId].value = opr;
    //cout << "opr = " << opr << endl; 
    //cout << "regId = " << regId << endl; 
    r[regId].isFree = false;
    return regId;
}

long long int RA_ensure(string opr) {
    long long int regId;
    for (long long int i = 0; i < NUMBER_REG; i++) {
        if (opr == r[i].value)
            return i;
    }
    regId = RA_allocate(opr);

    /* generate load from opr into r */
    TinyList.push_back( TinyCode("move", opr, "r"+to_string(regId)) );
    return regId;
}


//void IRCodeVar2TinyCodeStack(string &reg, const string &operand) {
long long int IRCodeVar2TinyCodeStack(string &reg, const string &operand, string flag) {
    // reg[0] = 'r';
    long long int regId = -1;
    if (operand[0] == '$') {
        if(operand[1] == 'L') {
            long long int tempCount;
            reg = "$-";
            istringstream(operand.substr(2, string::npos)) >> tempCount;
            reg += to_string(tempCount);
        }

        else if(operand[1] == 'P') {
            long long int tempCount;
            reg = "$";
            istringstream(operand.substr(2, string::npos)) >> tempCount;
            //tempCount = tempCount + tot_temp_vars + 1;
            tempCount = tempCount + NUMBER_REG + 1;
            reg += to_string(tempCount);
        }

        else if((operand[1] == 'T') && (flag == "src")) {
            long long int index;
            //reg = "r";
            istringstream(operand.substr(2, string::npos)) >> index;
            
            //reg += to_string(tempCount);
            //registerCount = tempCount;

            //regId = RA_ensure(operand);
            //regId = RA_ensure("$-" + to_string(FuncScope[currFunc].ParamCount + FuncScope[currFunc].LocalVarCount + index));
            regId = RA_ensure("$-" + to_string(FuncScope[currFunc].LocalVarCount + index - 1));
            reg = "r" + to_string(regId);
        }
        else if (operand[1] == 'T' && flag == "dst") {
             // Ashiwan Dec10 5 pm modified below to use $ even in dest
            long long int index;
            //reg = "r";
            istringstream(operand.substr(2, string::npos)) >> index;
            
            //regId = RA_allocate("$-" + to_string(FuncScope[currFunc].ParamCount + FuncScope[currFunc].LocalVarCount + index));
            //cout << "debug" << to_string(FuncScope[currFunc].LocalVarCount + index) << endl;
            regId = RA_allocate("$-" + to_string(FuncScope[currFunc].LocalVarCount + index - 1));

            //regId = RA_allocate(operand);
            reg = "r" + to_string(regId);
        }

        else if(operand[1] == 'R') {
            long long int tempCount;
            reg = "$";
            //cout << "tiny : curr func name " << currFunc << endl;
            //cout << "tiny : param count " << (FuncScope[currFunc].ParamCount -1) << endl;
            // ParamCount already has +1
            //tempCount = (tot_temp_vars + FuncScope[currFunc].ParamCount);
            tempCount = (NUMBER_REG + FuncScope[currFunc].ParamCount + 1);
            //cout << "FuncScope[currFunc].ParamCount = " << FuncScope[currFunc].ParamCount << endl;
            reg += to_string(tempCount);
        }
    }
    //else if (isNumber(operand)) { // numbers
    //// Ashiwan Dec10 5 pm just returning the number as such?? 
    //    //regId = RA_ensure(operand);
    //    //reg = "r" + to_string(regId);
    //    reg = operand;
    //} //Ashiwan Dec10 5 pm fixed to use src dst for global too
    //else if ((!isNumber(operand)) && (flag == "src")) { //global variable
    //    // may need to differentiate src and dst here
    //    regId = RA_ensure(operand);
    //    reg = "r" + to_string(regId);
    //}
    //else if ((!isNumber(operand)) && (flag == "dst")) { //global variable 
    //    regId = RA_allocate(operand);
    //    reg = "r" + to_string(regId);
    //}
    else {
        reg = operand;
    }
    //cout << ";[Debug] Give " << reg << " to " << operand << endl;

    return regId;
}


void GenerateTinyCodeFromIR() {
    VariableDeclFromSymbolTable();

    //list<IRNode>::iterator iter;
    //TinyNode temp;
    string reg1, reg2, reg_result;
    long long int reg1_id, reg2_id, reg_result_id, t_reg_id;
    reg1_id = -1;
    reg2_id = -1;
    reg_result_id = -1;
    t_reg_id = -1;

    for (map<string, list<ControlFlowGraphNode>>::const_iterator it = FuncCFGNodeList.begin(); it != FuncCFGNodeList.end(); it++) {
    //for (iter = IRList.begin(); iter != IRList.end(); iter++) {
        r[0].value = ""; r[0].isDirty = false; r[0].isBanned = false; r[0].isFree = true;
        r[1].value = ""; r[1].isDirty = false; r[1].isBanned = false; r[1].isFree = true;
        r[2].value = ""; r[2].isDirty = false; r[2].isBanned = false; r[2].isFree = true;
        r[3].value = ""; r[3].isDirty = false; r[3].isBanned = false; r[3].isFree = true;
        for (list<ControlFlowGraphNode>::const_iterator itt = it->second.begin(); itt != it->second.end(); itt++ ) {
            it_statement = itt;
            IRNode* ir_node = itt->node;

            /* Clear all the banned flag, since no register is being used by current IR code so far */
            /* Ashiwan Dec10 - step7 - why are we clearing for each IR code?? */
            
            for (long long int i = 0; i < NUMBER_REG; i++)
                r[i].isBanned = false;

            if (ir_node->opcode == "ADDI" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src"); // the last parameter is for differentiating source operand and destination operand
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                //if opr1, opr2 is dead, free opr1 and opr2
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                //Allocate reg_result
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                //Generate tiny code
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("addi", reg2, reg_result) );
                /* mark reg_result dirty */
                if (reg_result_id != -1)
                r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "ADDF" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");

                //cout << "debug " << reg_result << endl;
            
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("addr", reg2, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "SUBI" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("subi", reg2, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "SUBF" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("subr", reg2, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "MULTI" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("muli", reg2, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "MULTF" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("mulr", reg2, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "DIVI" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("divi", reg2, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "DIVF" ) {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1)
                    r[reg2_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                
                TinyList.push_back( TinyCode("move", reg1, reg_result) );
                TinyList.push_back( TinyCode("divr", reg2, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            // Ashiwan temporary dirty hack for step5 to fix a = b type code.
            // Not reusing registers. Just using a new register from global count
            else if (ir_node->opcode == "STOREI" || ir_node->opcode == "STOREF") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                if(reg1[0]!='r' && reg_result[0]!='r') {
                    //string t_reg = 'r' + to_string(++registerCount);
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("move", t_reg, reg_result ) );
                } else {
                    TinyList.push_back( TinyCode("move", reg1, reg_result) );
                }
                //TinyList.push_back( TinyCode("move", reg1, reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "GTI") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    //string t_reg = 'r' + to_string(++registerCount);
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                } else {
                    TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                TinyList.push_back( TinyCode("jgt", "", ir_node->result) );
            }
            else if (ir_node->opcode == "GTF") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    //string t_reg = 'r' + to_string(++registerCount);
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                } else {
                    TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                TinyList.push_back( TinyCode("jgt", "", ir_node->result) );
            }
            else if (ir_node->opcode == "GEI") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    //string t_reg = 'r' + to_string(++registerCount);
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                } else {
                    TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                TinyList.push_back( TinyCode("jge", "", ir_node->result) );
            }
            else if (ir_node->opcode == "GEF") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    //    string t_reg = 'r' + to_string(++registerCount);
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                } else {
                    TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                TinyList.push_back( TinyCode("jge", "", ir_node->result) );
            }
            else if (ir_node->opcode == "LTI") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                                    // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                } else {
                    TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                TinyList.push_back( TinyCode("jlt", "", ir_node->result) );
            }
            else if (ir_node->opcode == "LTF") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                               // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) ); 
 		} else {
                    TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                TinyList.push_back( TinyCode("jlt", "", ir_node->result) );
            }
            else if (ir_node->opcode == "LEI") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                               // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) ); 
		} else {
                    TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                TinyList.push_back( TinyCode("jle", "", ir_node->result) );
            }
            else if (ir_node->opcode == "LEF") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                               // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) ); 
		} else {
                    TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                TinyList.push_back( TinyCode("jle", "", ir_node->result) );
            }
            else if (ir_node->opcode == "EQI") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                    //TinyList.push_back( TinyCode("cmpi", reg2, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );                    
                } else {
                    TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                TinyList.push_back( TinyCode("jeq", "", ir_node->result) );
            }
            else if (ir_node->opcode == "EQF") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                                // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
		} else {
                    TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                TinyList.push_back( TinyCode("jeq", "", ir_node->result) );
            }
            else if (ir_node->opcode == "NEI") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                                // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpi", reg1, t_reg) );
		} else {
                    TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpi", reg1, reg2) );
                TinyList.push_back( TinyCode("jne", "", ir_node->result) );
            }
            else if (ir_node->opcode == "NEF") {
                reg1_id = IRCodeVar2TinyCodeStack(reg1, ir_node->operand_1, "src");
                if (reg1_id != -1)
                    r[reg1_id].isBanned = true;
                if (reg1_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                    RA_free(reg1_id);
                    r[reg1_id].isBanned = false;
                }
                reg2_id = IRCodeVar2TinyCodeStack(reg2, ir_node->operand_2, "src");
                if (reg2_id != -1 && itt->Live_OUT.find(ir_node->operand_2) == itt->Live_OUT.end()){ //opr2 is dead
                    RA_free(reg2_id);
                    r[reg2_id].isBanned = false;
                }
                if(reg1[0]!='r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) );
                    //    string t_reg = 'r' + to_string(++registerCount);
                               // Ashiwan fix to make second operand register
                } else if (reg1[0] == 'r' && reg2[0]!='r') {
                    string t_reg = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "dst");
                    //TinyList.push_back( TinyCode("move", reg1, t_reg ) );
                    TinyList.push_back( TinyCode("move", reg2, t_reg ) );
                    t_reg = "$T" + to_string(FuncScope[currFunc].tempVarCount);
                    t_reg_id = IRCodeVar2TinyCodeStack(t_reg, t_reg, "src");
                    if (t_reg_id != -1 && itt->Live_OUT.find(ir_node->operand_1) == itt->Live_OUT.end()){ //opr1 is dead
                        RA_free(t_reg_id);
                        r[t_reg_id].isBanned = false;
                    }
                    TinyList.push_back( TinyCode("cmpr", reg1, t_reg) ); 
		} else {
                TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                }
                //TinyList.push_back( TinyCode("cmpr", reg1, reg2) );
                TinyList.push_back( TinyCode("jne", "", ir_node->result) );
            }
            else if (ir_node->opcode == "READI" ) {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                TinyList.push_back( TinyCode("sys", "readi", reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "READF" ) {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                TinyList.push_back( TinyCode("sys", "readr", reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "READS" ) {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                TinyList.push_back( TinyCode("sys", "reads", reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "WRITEI" ) {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                TinyList.push_back( TinyCode("sys", "writei", reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "WRITEF" ) {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                TinyList.push_back( TinyCode("sys", "writer", reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "WRITES" ) {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "dst");
                TinyList.push_back( TinyCode("sys", "writes", reg_result) );
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "JUMP" ) {
                TinyList.push_back( TinyCode("jmp", "", ir_node->result) );
            }
            else if (ir_node->opcode == "LABEL" ) {
                TinyList.push_back( TinyCode("label", "", ir_node->result) );
                    if(ir_node->result.find("label") == std::string::npos)
                        currFunc = ir_node->result;
            }
            else if (ir_node->opcode == "PUSH") {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "src");
                if (isNumber(ir_node->result)) {
                    reg_result = "$T" + to_string(++FuncScope[currFunc].tempVarCount);
                    reg_result_id = IRCodeVar2TinyCodeStack(reg_result, reg_result, "src");
                    TinyList.push_back( TinyCode("move", ir_node->result, reg_result) );
                }
                TinyList.push_back( TinyCode("push", "", reg_result));
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "POP") {
                reg_result_id = IRCodeVar2TinyCodeStack(reg_result, ir_node->result, "src");
                TinyList.push_back( TinyCode("pop", "", reg_result));
                if (reg_result_id != -1)
                    r[reg_result_id].isDirty = true;
            }
            else if (ir_node->opcode == "LINK") {
                //cout << "tiny : curr func name " << currFunc << endl;
                //cout << "tiny : local var count " << (FuncScope[currFunc].LocalVarCount) << endl;
                //TinyList.push_back( TinyCode("link", "", to_string(FuncScope[currFunc].LocalVarCount)));
                TinyList.push_back( TinyCode("link", "", "100")); // ugly temp solution, don't know if it will work
            }
            else if (ir_node->opcode == "JSR") {
                for (long long int i = 0; i < NUMBER_REG; i++) {
                    //before CALL other function, free all global variables. 
                    if (r[i].value[0] != '$' && !isNumber(r[i].value)) {
                        RA_free(i);
                    }
                }
                PushAllRegistersToStack();
                TinyList.push_back( TinyCode("jsr", "", ir_node->result));
                PopAllRegistersFromStack();
            }
            else if (ir_node->opcode == "RET") {
                TinyList.push_back( TinyCode("unlnk", "", ""));
                TinyList.push_back( TinyCode("ret", "", ""));
            }
            //cout << "IR: " << ir_node->opcode << " " << ir_node->operand_1 << endl;
            //cout << "r0: " << r[0].value << " ";
            //cout << "r1: " << r[1].value << " ";
            //cout << "r2: " << r[2].value << " ";
            //cout << "r3: " << r[3].value << endl;
            //check if it is free, if so, remove value and mark it clean
            for (int i = 0; i < NUMBER_REG; i++) {
                if (r[i].isFree) {
                    r[i].value = "";
                    r[i].isDirty = false;
                }
            }
        }
        // need to flush used reg back
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
