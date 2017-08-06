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
   string var_name;
   string var_type;
   string value; 
} STE;


typedef struct SymbolTableScope 
{
   string scope_name;
   //Ashiwan added two labels to scope for step5
   vector <string> labels;
   string blk_type; 
   list<STE> SymbolTable; 
   
   SymbolTableScope()
   {
       scope_name = "GLOBAL";
   }
} Scope;

stack <Scope> SemanticStack;

stack <STE> temp_ids;

Scope globalScope;

void PrintAllSymbolTables() {
    list<STE>::iterator iter;
    stack <Scope> PrintStack;
    while(!SemanticStack.empty()) {
        PrintStack.push(SemanticStack.top());
        SemanticStack.pop();
    }
    while(!PrintStack.empty()) {
        Scope currScope = PrintStack.top();
        iter = currScope.SymbolTable.begin();
        cout << "Symbol table " << currScope.scope_name << endl; 
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


void CreateAndPushSymbolTableToStack(string scope, string my_blk_type) {
    Scope myscope;
    myscope.scope_name = scope;
    myscope.blk_type = my_blk_type;
    if(my_blk_type == "IF" || my_blk_type == "WHILE") {
        string lbl1 =  "label" + to_string(next_label_cnt + 1);  
        string lbl2 = "label" + to_string(next_label_cnt + 2);
        //cout << "Block : " << my_blk_type << " " << lbl1 << " " << lbl2 << endl;
        myscope.labels.push_back(lbl1);
        myscope.labels.push_back(lbl2);
        next_label_cnt += 2;
    } 
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
    //cout << endl;
    if (SemanticStack.top().scope_name == "GLOBAL")
    	globalScope = SemanticStack.top();
    //if (SemanticStack.top().SymbolTable.empty())
    //	cout<<"empty list"<<endl;
}


void AddStringSTEToSymbolTable(string varName, string varType, string val) {
    STE myste;
    myste.var_name = varName;
    myste.var_type = varType;
    myste.value = val;
    SemanticStack.top().SymbolTable.push_back(myste);
    if (SemanticStack.top().scope_name == "GLOBAL")
    	globalScope = SemanticStack.top();
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
