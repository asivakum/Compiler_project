#include <iostream>
#include <cstdlib>
#include "IRCodeGeneration.h"

using namespace std;

class ControlFlowGraphNode {
public:
	IRNode *node;
	list<ControlFlowGraphNode *> predecessor_list;
	list<ControlFlowGraphNode *> successor_list;
	bool is_leader;

	ControlFlowGraphNode() {
		node = NULL; 
		is_leader = false;
	}
};

map<string, list<ControlFlowGraphNode>> FuncCFGNodeList;

void ControlFlowGraphGeneration(string funcName) {
	map<string, *ControlFlowGraphNode> labelMapping;

	if (FuncIRList.find(funcName) == map::end) {
		cerr << "[ERROR] Function name not found in FuncIRList" << endl;
		abort();
	}
	/* 1st iteration */
	for (list::const_iterator it = FuncIRList[funcName].begin(); it != FuncIRList[funcName].end(); it++) {
		ControlFlowGraphNode *temp = new ControlFlowGraphNode();
		temp -> node = &(*it);
		if (it != FuncIRList[funcName].begin()) {
			temp -> predecessor_list.push_back( &(FuncCFGNodeList[funcName].back()) );
			FuncCFGNodeList[funcName].push_back(*temp);
			( temp -> predecessor_list.front() ) -> successor_list.push_back( &(FuncCFGNodeList[funcName].back()) );
		}
		if (*it.opcode == "LABEL") {
			labelMapping[*it.result] = &(FuncCFGNodeList[funcName].back());
		}
		delete temp;
	}
	/* 2nd iteration */
	for (list::const_iterator it = FuncCFGNodeList[funcName].begin(); it != FuncCFGNodeList[funcName].end(); it++) {
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
