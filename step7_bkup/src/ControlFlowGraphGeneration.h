#include <iostream>
#include <cstdlib>
#include "IRCodeGeneration.h"
#include <set>
#include <algorithm>

using namespace std;

class ControlFlowGraphNode {
public:
	IRNode *node;
	list<ControlFlowGraphNode *> predecessor_list;
	list<ControlFlowGraphNode *> successor_list;
	bool is_leader;
        set<string> GEN_SET;
        set<string> KILL_SET;
        set<string> Live_IN;
        set<string> Live_OUT;

	ControlFlowGraphNode() {
		node = NULL; 
		is_leader = false;
    	        GEN_SET = {};
	        KILL_SET = {};
                Live_IN = {};
                Live_OUT = {};
	}
};

map<string, list<ControlFlowGraphNode>> FuncCFGNodeList;


void ControlFlowGraphGeneration(string funcName) {
	// Ashiwan changed below to ControlFlowGraphNode*
	//map<string, *ControlFlowGraphNode> labelMapping;
	map<string, ControlFlowGraphNode*> labelMapping;

	// Ashiwan changed below
	//if (FuncIRList.find(funcName) == map::end) {
	if (FuncIRList.find(funcName) == FuncIRList.end()) {
		cerr << "[ERROR] Function name not found in FuncIRList" << endl;
		abort();
	}
	
        /* 1st iteration */
	for (list<IRNode>::iterator it = FuncIRList[funcName].begin(); it != FuncIRList[funcName].end(); it++) {
		ControlFlowGraphNode *temp = new ControlFlowGraphNode();
		temp -> node = &(*it);
		if (it != FuncIRList[funcName].begin()) {
			temp -> predecessor_list.push_back( &(FuncCFGNodeList[funcName].back()) );
			FuncCFGNodeList[funcName].push_back(*temp);
			( temp -> predecessor_list.front() ) -> successor_list.push_back( &(FuncCFGNodeList[funcName].back()) );
		} else {
			FuncCFGNodeList[funcName].push_back(*temp);
                }
		if (it->opcode == "LABEL") {
			labelMapping[it->result] = &(FuncCFGNodeList[funcName].back());
		}
		delete temp;
	}
	/* 2nd iteration */
	for (list<ControlFlowGraphNode>::iterator it = FuncCFGNodeList[funcName].begin(); it != FuncCFGNodeList[funcName].end(); it++) {
		string opcode = it -> node -> opcode;
		if (opcode == "GEI" || opcode == "GEF" || opcode == "GTI" || opcode == "GTF" || opcode == "LEI" || opcode == "LEF" || opcode == "LTI" || opcode == "LTF" || opcode == "NEI" || opcode == "NEF" || opcode == "EQI" || opcode == "EQF") {
			it -> successor_list.push_back(labelMapping[it -> node -> result]);
			labelMapping[it -> node -> result] -> predecessor_list.push_back(&(*it));
		}
		else if (opcode == "JUMP") {
			it -> successor_list.clear();
			it -> successor_list.push_back(labelMapping[it -> node -> result]);
		}
	}
}


void PopulateGenAndKillSet(string funcName) {

	if (FuncCFGNodeList.find(funcName) == FuncCFGNodeList.end()) {
		cerr << "[ERROR] Function name not found in FuncCFGNodeList" << endl;
		abort();
	}
        //cout << "Function : " << funcName << endl; 
	for (list<ControlFlowGraphNode>::iterator it = FuncCFGNodeList[funcName].begin(); it != FuncCFGNodeList[funcName].end(); it++) {
               //cout << "Instruction : " << it->node->opcode << endl;
		if (it->node->opcode == "") {
		    cerr << "[ERROR] Opcode is null. Can't get here" << endl;
		    abort();
		}
                string myopcode = it->node->opcode;
                // Ignore JUMP and LABEL. All other statements find GEN and KILL sets
                if (myopcode == "ADDI" || myopcode == "ADDF" || myopcode == "SUBI" || myopcode == "SUBF" || myopcode == "MULTI" || myopcode == "MULTF" || myopcode == "DIVI" || myopcode == "DIVF") {
                    if(it->node->operand_1[0] == '$')
                        it->GEN_SET.insert(it->node->operand_1);
                    if(it->node->operand_2[0] == '$')
                        it->GEN_SET.insert(it->node->operand_2);
                    if(it->node->result[0] == '$')
                        it->KILL_SET.insert(it->node->result);
                } else if (myopcode == "STOREI" || myopcode == "STOREF") {
                    if(it->node->operand_1[0] == '$')
                        it->GEN_SET.insert(it->node->operand_1);
                    if(it->node->result[0] == '$')
                        it->KILL_SET.insert(it->node->result);

                } else if (myopcode == "PUSH" || myopcode == "WRITEI" || myopcode == "WRITEF" || myopcode == "WRITES") {
                    if(it->node->result[0] == '$')
                        it->KILL_SET.insert(it->node->result);
                } else if (myopcode == "POP" || myopcode == "READI" || myopcode == "READF" || myopcode == "READS") {
                    if(it->node->result[0] == '$')
                        it->GEN_SET.insert(it->node->result);
                } else if (myopcode == "GEI" || myopcode == "GEF" || myopcode == "GTI" || myopcode == "GTF" || myopcode == "LEI" || myopcode == "LEF" || myopcode == "LTI" || myopcode == "LTF" || myopcode == "NEI" || myopcode == "NEF" || myopcode == "EQI" || myopcode == "EQF") {
                    if(it->node->operand_1[0] == '$')
                        it->GEN_SET.insert(it->node->operand_1);
                    if(it->node->operand_2[0] == '$')
                        it->GEN_SET.insert(it->node->operand_2);
                } else if (myopcode == "JSR") {
                    for(list<STE>::iterator iter = globalScope.SymbolTable.begin(); iter!= globalScope.SymbolTable.end(); iter++)
                        it->GEN_SET.insert(iter->var_name);
                }
                //cout << "GEN SET: ";
                //for (set<string>::iterator itt = it->GEN_SET.begin(); itt!=it->GEN_SET.end(); itt++)
                //    cout << *itt;
                //cout << endl;
                //cout << "KILL SET: ";
                //for (set<string>::iterator itt = it->KILL_SET.begin(); itt!=it->KILL_SET.end(); itt++)
                //    cout << *itt;
                //cout << endl;
	}
}

void LivenessAnalysis(string funcName) {
	//Initialize the OUT_SET of all RET nodes to global
	if (FuncCFGNodeList.find(funcName) == FuncCFGNodeList.end()) {
		cerr << "[ERROR] Function name not found in FuncCFGNodeList" << endl;
		abort();
	}
        //cout << "Function : " << funcName << endl;
        list<ControlFlowGraphNode> WorkList;
	for (list<ControlFlowGraphNode>::iterator it = FuncCFGNodeList[funcName].begin(); it != FuncCFGNodeList[funcName].end(); it++) {
               //cout << "Instruction : " << it->node->opcode << endl;
		if (it->node->opcode == "") {
		    cerr << "[ERROR] Opcode is null. Can't get here" << endl;
		    abort();
		}
                string myopcode = it->node->opcode;
                if (myopcode == "RET") {
                    for(list<STE>::iterator iter = globalScope.SymbolTable.begin(); iter!= globalScope.SymbolTable.end(); iter++)
                        it->Live_OUT.insert(iter->var_name);
                }
                // Push the nodes to the worklist
                WorkList.push_back(*it);
        }
        
        while(!WorkList.empty()) {
            ControlFlowGraphNode temp = WorkList.front();
            set<string> tempLiveOut = {};
            set<string> tempLiveIn  = {};
            set<string> NewLiveOut = {};
            set<string> NewLiveIn  = {};
            cout << "WorkList front : " << temp.node->opcode << endl;
            // Liveout is union of all successors
            for (list<ControlFlowGraphNode*>::iterator it = temp.successor_list.begin(); it != temp.successor_list.end(); it++) {
                cout << "Successor : " << it->node->opcode << endl;
	        set_union(it->Live_IN.begin(), it->Live_IN.end(), NewLiveOut.begin(), NewLiveOut.end(), inserter(tempLiveOut, tempLiveOut.begin()));
	        NewLiveOut.swap(tempLiveOut); //tempLiveOut deleted?	
	    }
            //Livein is Liveout - killset + genset
            copy(NewLiveOut.begin(), NewLiveOut.end(), tempLiveIn);
            for (set<string>::iterator itt = temp.KILL_SET.begin(); itt!=temp.KILL_SET.end(); itt++) {
                tempLiveIn.erase(*itt); 
	    }
            set_union(tempLiveIn.begin(), tempLiveIn.end(), temp.GEN_SET.begin(), temp.GEN_SET.end(), inserter(NewLiveIn, NewLiveIn.begin()));
            //If livein has changed, push all predecessors to worklist
            if (temp.Live_IN != NewLiveIn) {
	        for (list<ControlFlowGraphNode*>::iterator it = temp.predecessor_list.begin(); it != temp.predecessor_list.end(); it++) {
		    WorkList.push_back(*it);	
                }
		temp.Live_IN.swap(NewLiveIn); 
                temp.Live_OUT.swap(NewLiveOut); 
	    }
	}
        WorkList.pop_front();
}
