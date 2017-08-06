#ifndef STE_HH
# define STE_HH
# include <string>
# include <stack>
# include <list>
# include <vector>
# include <map>

using namespace std;
int block_cnt = 0;
long long int next_label_cnt = 0;
string blk_scope_name = "BLOCK ";

typedef struct SemanticRecord 
{
   //Ashiwan added new IR var name for step6
   string var_name;
   string var_type;
   string value;
   string IR_var_name; 
} STE;


typedef struct SymbolTableScope 
{
   string scope_name;
   string blk_type;
   string ret_type;

   //Ashiwan added two labels to scope for step5
   vector <string> labels;
   list<STE> SymbolTable; 
   //Ashiwan added scope specific var counts and IRCode list for step6
   long long int LocalVarCount;
   long long int ParamCount;
   long long int tempVarCount;   
   SymbolTableScope()
   {
       scope_name = "GLOBAL";
       LocalVarCount = 1;
       ParamCount = 1;
       tempVarCount = 1;
   }
} Scope;

stack <Scope> ScopeStack;
map<string, Scope> FuncScope;
string currFuncName = "";
list <string> FuncNames;
stack <STE> temp_ids;

Scope globalScope;

//Ashiwan step6 checking global and local scopes for IR Var names. In step7 we might need to keep a list of scopes and look through enclosed nested?
//
string GetIRVarName(string literal) {
    list<STE>::iterator iter;
    Scope currScope = FuncScope[currFuncName];
    //cout << "GetIRVarName symbol table " << currScope.scope_name << " " << currFuncName << endl; 
    if(currFuncName == "global")
        return literal;
    if((literal.find("$T") != string::npos) || (literal.find("$P") != string::npos) || (literal.find("$L") != string::npos))
        return literal;
    iter = globalScope.SymbolTable.begin();
    //cout << "Literal " << literal << endl;
    while(iter != globalScope.SymbolTable.end()) {
        //cout << "Iter->var_name " << iter->var_name << endl;
        if(iter->var_name == literal)
            return iter->var_name;
        iter++;
    }
    iter = currScope.SymbolTable.begin();
    //cout << "Literal " << literal << endl;
    while(iter != currScope.SymbolTable.end()) {
        //cout << "Iter->var_name " << iter->var_name << endl;
        if(iter->var_name == literal)
            return iter->IR_var_name;
        iter++;
    }
    return "\0";
}


string SearchSymbolTableForType(string id) {
    
    //cout << "Srch ScopeStack.top() " << ScopeStack.top().scope_name << " " << ScopeStack.top().blk_type << endl;
    /* Search current Scope */
    list<STE>::iterator iter;
    iter = ScopeStack.top().SymbolTable.begin();
    //cout<<"id:"<<id<<endl;
    //if (ScopeStack.top().SymbolTable.empty())
    //	cout<<"empty"<<endl;
    //cout<<"stack: "<<iter->var_name<<endl;
    while(iter != ScopeStack.top().SymbolTable.end()) {
    	//cout<<"stack: "<<iter->var_name<<endl;
    	if (iter->var_name == id) {
	    return iter->var_type;
	}
    	iter++;
    }
    /* Search global Scope */
    iter = globalScope.SymbolTable.begin();
    while(iter != globalScope.SymbolTable.end()) {
    	//cout<<"stack: "<<iter->var_name<<endl;
    	if (iter->var_name == id) {
	    return iter->var_type;
	}
    	iter++;
    }
    // Also search current functions symbol table
    iter = FuncScope[currFuncName].SymbolTable.begin();
    while(iter != FuncScope[currFuncName].SymbolTable.end()) {
    	if (iter->var_name == id) {
	    return iter->var_type;
	}
    	iter++;
    }

    return "\0";
}

string SearchCurrFuncSymbolTableForType(string id) {
    list<STE>::iterator iter;
    iter = FuncScope[currFuncName].SymbolTable.begin();
    while(iter != FuncScope[currFuncName].SymbolTable.end()) {
    	if (iter->var_name == id) {
	    return iter->var_type;
	}
    	iter++;
    }
    return "\0";
}


vector<string> SearchSymbolTableForLabel(string my_blk_type) {
    if(my_blk_type == "IF" || my_blk_type == "WHILE")
        return ScopeStack.top().labels;
    else if(my_blk_type == "ELSE") {
        Scope temp = ScopeStack.top();
        ScopeStack.pop();
        vector<string> labels = ScopeStack.top().labels;
        ScopeStack.push(temp);
        return labels;
     }
}

void PrintAllSymbolTables() {
    list<STE>::iterator iter;
    stack <Scope> PrintStack;
    //while(!ScopeStack.empty()) {
    //    PrintStack.push(ScopeStack.top());
    //    ScopeStack.pop();
    //}
    PrintStack = ScopeStack;
    while(!PrintStack.empty()) {
        Scope currScope = PrintStack.top();
        iter = currScope.SymbolTable.begin();
        cout << "Symbol table " << currScope.scope_name << endl; 
        while(iter != currScope.SymbolTable.end()) {
            if(iter->var_type == "STRING") {
                cout << "name " << iter->var_name << " and " << iter->IR_var_name << " type " << iter->var_type << " value " << iter->value << endl;
            } else {
                cout << "name " << iter->var_name << " and " << iter->IR_var_name << " type " << iter->var_type << endl;
            }
            iter++;
        }
        PrintStack.pop();
        cout << endl;
    }
}

void CreateAndPushSymbolTableToStack(string scope, string my_blk_type, string my_ret_type) {
    Scope myscope;
    myscope.scope_name = scope;
    myscope.blk_type = my_blk_type;
    myscope.ret_type = my_ret_type;
    if(my_blk_type == "IF" || my_blk_type == "WHILE") {
        string lbl1 =  "label_" + to_string(next_label_cnt + 1);  
        string lbl2 = "label_" + to_string(next_label_cnt + 2);
        //cout << "Block : " << my_blk_type << " " << lbl1 << " " << lbl2 << endl;
        myscope.labels.push_back(lbl1);
        myscope.labels.push_back(lbl2);
        next_label_cnt += 2;
    } 
    if(my_blk_type != "IF" && my_blk_type != "WHILE" && my_blk_type != "ELSE") {
        currFuncName = my_blk_type;
        FuncScope[currFuncName] = myscope;
        FuncNames.push_back(my_blk_type);
    }
    ScopeStack.push(myscope);
    //return myscope;
}

void UnknownIDTypeList(string varName, string varScopeStr) {
    string id_type = SearchSymbolTableForType(varName);
        
    STE tmp_ste;
    Scope currScope;
    string ir_var_name;
    tmp_ste.var_name = varName;
    tmp_ste.var_type = "null";
    if(currFuncName != "global") {
        currScope = FuncScope[currFuncName];
        if(varScopeStr == "$P") {
            ir_var_name = varScopeStr + to_string(currScope.ParamCount);
            FuncScope[currFuncName].ParamCount++;
            ScopeStack.top().ParamCount++;
        }
        else if(varScopeStr == "$L") {
            ir_var_name = varScopeStr + to_string(currScope.LocalVarCount);
            if(id_type.empty()) {
                FuncScope[currFuncName].LocalVarCount++;
                ScopeStack.top().LocalVarCount++;
            }
        }
        tmp_ste.IR_var_name = ir_var_name;
    }
    temp_ids.push(tmp_ste); 
}

void AddSTEToSymbolTable(STE val) {
    ScopeStack.top().SymbolTable.push_back(val);
    FuncScope[currFuncName].SymbolTable.push_back(val);
    //cout << endl;
    if (ScopeStack.top().scope_name == "GLOBAL")
    	globalScope = ScopeStack.top();
    //if (ScopeStack.top().SymbolTable.empty())
    //	cout<<"empty list"<<endl;
}


void AddStringSTEToSymbolTable(string varName, string varType, string val, string varScopeStr) {
    STE myste;
    Scope currScope;
    string ir_var_name;
    myste.var_name = varName;
    myste.var_type = varType;
    myste.value = val;
    if(currFuncName != "global") {
        currScope = FuncScope[currFuncName];
        if(varScopeStr == "$P") {
            ir_var_name = varScopeStr + to_string(currScope.ParamCount);
            FuncScope[currFuncName].ParamCount++;
            ScopeStack.top().ParamCount++;
        }
        else if(varScopeStr == "$L") {
            ir_var_name = varScopeStr + to_string(currScope.LocalVarCount);
            FuncScope[currFuncName].LocalVarCount++;
            ScopeStack.top().LocalVarCount++;
        }
        myste.IR_var_name = ir_var_name;
    }
    ScopeStack.top().SymbolTable.push_back(myste);
    FuncScope[currFuncName].SymbolTable.push_back(myste);
    if (ScopeStack.top().scope_name == "GLOBAL")
    	globalScope = ScopeStack.top();
}

void AddParamSTEToSymbolTable(string varName, string varType, string val, string varScopeStr) {
    STE myste;
    Scope currScope;
    string ir_var_name;
    myste.var_name = varName;
    myste.var_type = varType;
    myste.value = val;
    if(currFuncName != "global") {
        currScope = FuncScope[currFuncName];
        ir_var_name = varScopeStr + to_string(currScope.ParamCount);
        ScopeStack.top().ParamCount++;
        FuncScope[currFuncName].ParamCount++;
        myste.IR_var_name = ir_var_name;
    }
    ScopeStack.top().SymbolTable.push_back(myste);
    FuncScope[currFuncName].SymbolTable.push_back(myste);
    if (ScopeStack.top().scope_name == "GLOBAL")
    	globalScope = ScopeStack.top();
}


int CheckDeclError() {
    list<STE>::iterator iter;
    stack <Scope> tempStack;
    tempStack = ScopeStack;
    int ret_val = 0;
    while(!tempStack.empty()) {
        map<string, int> cnt_map;
        Scope currScope = tempStack.top();
        iter = currScope.SymbolTable.begin();
        while(iter != currScope.SymbolTable.end()) {
            if(cnt_map.find(iter->var_name) == cnt_map.end()) {
                cnt_map[iter->var_name] = 1;
            } else {
                cout << "DECLARATION ERROR " << iter->var_name << endl; 
                ret_val = 1;
                break;
            }
            iter++;
        }
        if(ret_val == 1) {
            break;
        } else {
            tempStack.pop();
        }
    }
    return ret_val;
}
 

void CheckShadowWarning() {
    list<STE>::iterator iter;
    stack <Scope> tempStack;
    tempStack = ScopeStack;
    map<string, int> cnt_map;
    while(!tempStack.empty()) {
        Scope currScope = tempStack.top();
        iter = currScope.SymbolTable.begin();
        while(iter != currScope.SymbolTable.end()) {
            if(cnt_map.find(iter->var_name) == cnt_map.end()) {
                cnt_map[iter->var_name] = 1;
            } else {
                cout << "SHADOW WARNING " << iter->var_name << endl; 
                cnt_map[iter->var_name] += 1;
            }
            iter++;
        }
        tempStack.pop();
    }
}

#endif // ! STE_HH
