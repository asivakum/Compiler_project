#ifndef STE_HH
# define STE_HH
# include <string>
# include <stack>
# include <list>
# include <map>

using namespace std;
int block_cnt = 0;
string blk_scope_name = "BLOCK ";

typedef struct SemanticRecord 
{
   string var_name;
   string var_type;
   string value; 
} STE;


typedef struct SymbolTableScope 
{
   string scope_name;
   list<STE> SymbolTable; 
   
   SymbolTableScope()
   {
       scope_name = "GLOBAL";
   }
} Scope;

stack <Scope> SemanticStack;

stack <STE> temp_ids;

void PrintAllSymbolTables() {
    list<STE>::iterator iter;
    stack <Scope> PrintStack;
    while(!SemanticStack.empty()) {
        PrintStack.push(SemanticStack.top());
        SemanticStack.pop();
    }
    while(!PrintStack.empty()) {
        Scope currScope = PrintStack.top();
        //iter = Global.SymbolTable.begin();
        iter = currScope.SymbolTable.begin();
        cout << "Symbol table " << currScope.scope_name << endl; 
        //while(iter != Global.SymbolTable.end()) {
        while(iter != currScope.SymbolTable.end()) {
            if(iter->var_type == "STRING") {
                cout << "name " << iter->var_name << " type " << iter->var_type << " value " << iter->value << endl;
            } else {
                cout << "name " << iter->var_name << " type " << iter->var_type << endl;
            }
            iter++;
        }
        PrintStack.pop();
        cout << endl;
    }
}


void CreateAndPushSymbolTableToStack(string scope) {
    Scope myscope;
    myscope.scope_name = scope;
    SemanticStack.push(myscope); 
}

void UnknownIDTypeList(string varName) {
    STE tmp_ste;
    tmp_ste.var_name = varName;
    tmp_ste.var_type = "null";
    temp_ids.push(tmp_ste); 
}

void AddSTEToSymbolTable(STE val) {
    SemanticStack.top().SymbolTable.push_back(val);
}


void AddStringSTEToSymbolTable(string varName, string varType, string val) {
    STE myste;
    myste.var_name = varName;
    myste.var_type = varType;
    myste.value = val;
    SemanticStack.top().SymbolTable.push_back(myste);
}

int CheckDeclError() {
    list<STE>::iterator iter;
    stack <Scope> tempStack;
    tempStack = SemanticStack;
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
    tempStack = SemanticStack;
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
