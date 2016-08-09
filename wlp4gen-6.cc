#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <stdlib.h>     /* exit, EXIT_FAILURE */

#include <sstream>

#include <string>

using namespace std;


struct Tree {
    string rule;
    string type;
    vector<string> tokens;
    vector<Tree *> children;
};

vector<string> split_line(string line) {
    vector<string> ret;
    string s = "";
    //cerr <<"line.size: " <<line.size()<< endl;
    for (int i = 0; i < line.size(); ++ i) {
        if(i==line.size()-1){
            s +=line[i];
            //cerr << "last: " <<"i: "<<i<< s<<endl;
            ret.push_back(s);
        }
        
        else if (line[i] != ' '){
            s += line[i];
            //cerr <<"i: "<< i << "  s: "<< s<<endl;
        }
        else {
            ret.push_back(s);
            //cerr <<"i: "<< i << "  s: "<< s<<endl;
            s = "";
        }
    }
    for(int i =0; i < ret.size(); ++i){
   // cerr <<i<< ": aaaa"<< ret[i] << endl;
    }
    return ret;
}

bool is_last_rule(string words) {
    for (int i = 0; i < words.size(); ++ i)
        if (!(words[i] >= 'A' && words[i] <= 'Z')) return false;
    return true;
}

Tree *BuildParseTree() {
    Tree *T = new Tree();
    string line;
    getline(cin, line);

    T->rule = line;
    vector<string> words =  split_line(line);
 

    T->type = words[0];
    //T->tokens = words;
    
    if(words.size()==1){
      //  T->tokens.push_back(words[0]);
        return T;
    }
    for(int i =1; i< words.size();++i){
        //string one = words[i];
       // T->tokens[i]=one;
        
        T->tokens.push_back(words[i]);
        //cerr << i<<"i:  "<<T->tokens[i-1] <<endl;
        }
        //cerr << "fuuuuuuuck : "<<T->tokens.size() << endl;
        //cerr << T->tokens.size() << endl;
        // this cannot be 0
        T->children.clear();
        if (is_last_rule(T->type)) return T;
        for (int i = 1; i < words.size(); ++ i) {
        //cerr <<"children "<<i<<" :"<< T->children[i]->type<<endl;
            T->children.push_back(BuildParseTree());
        }
        return T;
    }

Tree *parse_tree;



void procedures_traverse(Tree *t); 
void procedure_traverse(Tree *t);    // to do
void main_traverse(Tree *t); // to do 
// params to do
void params_traverse(Tree *t, map<string, pair<string,int> > &var_table, vector<string> &par_name, vector<string> &par_type);  
void paramlist_traverse(Tree *t, map<string, pair<string,int> > &var_table, vector<string> &par_name, vector<string> &par_type); 
//void type_traverse(Tree *t, map<string, string> &var_table);

void dcls_traverse(Tree *t, map<string, pair<string,int> > &var_table);
void dcl_traverse(Tree *t, map<string, pair<string,int> > &var_table);  // to do
void statements_traverse(Tree *t, map<string, pair<string,int> > &var_table);
void statement_traverse(Tree *t, map<string, pair<string,int> > &var_table);  
void test_traverse(Tree *t, map<string, pair<string,int> > &var_table);    
void expr_traverse(Tree *t, map<string, pair<string,int> > &var_table);
void term_traverse(Tree *t, map<string, pair<string,int> > &var_table);
void factor_traverse(Tree *t, map<string, pair<string,int> > &var_table);  
void arglist_traverse(Tree *t, map<string, pair<string,int> > &var_table); 
void lvalue_traverse(Tree *t, map<string, pair<string,int> > &var_table); 

int offset;
map<string, pair< vector<string>, map<string, pair<string,int> > > > topsymtbl;
    //proc name      types of prarameter   var      type   location

//map<string, pair< vector<string>, map<string, string> > > topsymtbl;
     //proc name      types of prarameter   var  type


void params_traverse(Tree *t, map<string, pair<string,int> > &var_table, vector<string> &par_name, vector<string> &par_type ){
    if(t->children.size()==0 ){
        return;
    }else {
        paramlist_traverse(t->children[0],var_table, par_name, par_type );
    }
}
void paramlist_traverse(Tree *t, map<string, pair<string,int> > &var_table,  vector<string> &par_name, vector<string> &par_type){
    dcl_traverse(t->children[0],var_table);
    string id, type;
    id = t->children[0]->children[1]->tokens[0];
    if(t->children[0]->children[0]->children.size()==2){
        type = "int*";
        //cerr <<id<<" int*"<<endl;
    } else {
        type = "int";
        //cerr <<id <<" int"<<endl;
    }
    par_name.push_back(id);
    par_type.push_back(type);
    
    if(t->children.size() != 1 ){
        paramlist_traverse(t->children[2],var_table,par_name,par_type);
    }
}


void test_traverse(Tree *t, map<string, pair<string,int> > &var_table){
    expr_traverse(t->children[0],var_table);
    expr_traverse(t->children[2],var_table);
}
void arglist_traverse(Tree *t, map<string, pair<string,int> > &var_table){
    if(t->children.size()==1 ){
        expr_traverse(t->children[0],var_table);
    }else {
        expr_traverse(t->children[0],var_table);
        arglist_traverse(t->children[2],var_table);
    }
}

void lvalue_traverse(Tree *t, map<string, pair<string,int> > &var_table){
    if(t->children[0]->type == "ID"){
        if(var_table.count(t->children[0]->tokens[0]) == 0){
            cerr<<"ERROR"<<endl;
            exit(EXIT_FAILURE);
        }
    }else if(t->children[0]->type == "STAR"){
        factor_traverse(t->children[1], var_table);

    }else if(t->children[0]->type == "LPAREN"){
        lvalue_traverse(t->children[1], var_table);
    }
}

void procedures_traverse(Tree *t){
    if(t->children[0]->type == "main"){
        main_traverse(t->children[0]);
    }else {
        procedure_traverse(t->children[0]);
        procedures_traverse(t->children[1]);
    }
}

void dcls_traverse(Tree *t, map<string, pair<string,int> > &var_table){
    if(t->children.size()==0 ){
        return;
    }else {
        dcls_traverse(t->children[0], var_table);
        dcl_traverse(t->children[1], var_table);
    }
}


void procedure_traverse(Tree *t) {
    string id = t->children[1]->tokens[0];
    if(topsymtbl.count(id) > 0 ){
        cerr<<"ERROR"<<endl;
        exit(EXIT_FAILURE);
    }
    map<string, pair<string,int> > var_table;  
    vector<string> par_type;
    vector<string> par_name;
    pair< vector<string>, map<string, pair<string,int> > > p;
    p.first = par_name;
    p.second = var_table;

    topsymtbl.insert(pair <string,pair< vector<string>, map<string, pair<string,int> > > >(id, p));
    //topsymtbl.insert(pair<string,pair< vector<string>, map<string, string> > >(id, p));

    //params
    params_traverse(t->children[3], var_table, par_name, par_type);
    //dlcs
    dcls_traverse(t->children[6], var_table);
    //statmenets
    statements_traverse(t->children[7], var_table);
    //expr
    expr_traverse(t->children[9], var_table);
   // // should cerr 
   /*
    cerr << id;
    for (std::vector<string>::const_iterator i = par_type.begin(); i != par_type.end(); ++i){
       cerr << " " << *i;
    }
    cerr <<endl ;

    for(map<string, string> ::const_iterator it = var_table.begin(); it != var_table.end(); ++it)
    {
        cerr << it->first << " " << it->second << "\n";
    }
    cerr <<endl ;
    */
    topsymtbl[id] =  make_pair(par_name, var_table);     
    offset = 0;
   
}

void main_traverse(Tree *t) {
    //if((t->type =="main")||(t->type =="procedure")){
    if(topsymtbl.count("wain") >0 ){
        cerr<<"ERROR"<<endl;
        exit(EXIT_FAILURE);
    }
    map<string, pair<string,int> > var_table;  
       // routine_traverse(t, var_table);
    //dcl_...
    dcl_traverse(t->children[3], var_table);
    dcl_traverse(t->children[5], var_table);
    //dlcs
    dcls_traverse(t->children[8], var_table);
    //statmenets
    statements_traverse(t->children[9], var_table);
    //expr
    expr_traverse(t->children[11], var_table);

    vector<string> par_name;
    string id1 = t->children.at(3)->children.at(1)->tokens[0];
    string id2 = t->children.at(5)->children.at(1)->tokens[0];
    // should cerr
    /*    
    cerr << "wain "<< var_table[id1] << " " << var_table[id2] << endl;
    for(map<string, string> ::const_iterator it = var_table.begin(); it != var_table.end(); ++it)
    {
        //cerr<<"aaa "<<endl;
        cerr << it->first << " " << it->second << "\n";
    }
    */
    par_name.push_back(id1);
    par_name.push_back(id2); 

    pair< vector<string>, map<string, pair<string,int> > > p(par_name, var_table);

    topsymtbl["wain"] = p;
    //topsymtbl.insert(pair<string,pair< vector<string>, map<string, string> > >("wain", p));
    offset = 0;
}

void dcl_traverse(Tree *t, map<string, pair<string,int> > &var_table) {
    string id, type;
    id = t->children[1]->tokens[0];
    if(var_table.count(id) > 0){
        cerr<<"ERROR"<<endl;
        exit(EXIT_FAILURE);
    }
    if(t->children[0]->children.size()==2){
        type = "int*";
        //cerr <<id<<" int*"<<endl;
    } else {
        type = "int";
        //cerr <<id <<" int"<<endl;
    }
    pair<string,int> var = make_pair(type, offset);
    offset -= 4;
    var_table.insert( pair<string, pair<string,int> > (id,var));
}

void term_traverse(Tree *t, map<string, pair<string,int> > &var_table) {
    if(t->children[0]->type=="term"){ 
        term_traverse(t->children[0],var_table);
        factor_traverse(t->children[2],var_table);

    }else {
        factor_traverse(t->children[0],var_table);
    }
    
    }
void expr_traverse(Tree *t, map<string, pair<string,int> > &var_table) {
   // cout << t->children[0]->tokens[0] << endl;
   if(t->children[0]->type=="term"){ 
    term_traverse(t->children[0],var_table);}
   else {
        expr_traverse(t->children[0],var_table);
        term_traverse(t->children[2],var_table);
    }

}
void factor_traverse(Tree *t, map<string, pair<string,int> > &var_table) {
    if(t->children[0]->type=="ID"){ 
        if(t->children.size()==1){
            if(var_table.count(t->children[0]->tokens[0]) == 0){
                cerr<<"ERROR"<<endl;
                exit(EXIT_FAILURE);
            }
        }else if (t->children.size()==3){
            if(topsymtbl.count(t->children[0]->tokens[0]) == 0){
                cerr<<"ERROR"<<endl;
                exit(EXIT_FAILURE);
            }
        }else {
            if(topsymtbl.count(t->children[0]->tokens[0]) == 0){
                cerr<<"ERROR"<<endl;
                exit(EXIT_FAILURE);
            }
            arglist_traverse(t->children[2],var_table);


        }
        //cerr<<"ID"<<t->children[0]->tokens[0]<<endl;
    }else if(t->children[0]->type=="LPAREN"){ 
        expr_traverse(t->children[1],var_table);

    }else if(t->children[0]->type=="AMP"){ 
        lvalue_traverse(t->children[1],var_table);

    }else if(t->children[0]->type=="STAR"){ 
        factor_traverse(t->children[1],var_table);
    }else if(t->children[0]->type=="NEW"){ 
        expr_traverse(t->children[3],var_table);

    }else if(t->children[0]->type=="NUM"){ 
        
    }else if(t->children[0]->type=="NULL"){ 

    }
}

void statements_traverse(Tree *t, map<string, pair<string,int> > &var_table){
    if(t->children.size()==0){
        return;
    }else {
        statements_traverse(t->children[0],var_table);
        statement_traverse(t->children[1],var_table);
    }
}

void statement_traverse(Tree *t, map<string, pair<string,int> > &var_table){
    if(t->children[0]->type=="lvalue"){
        lvalue_traverse(t->children[0],var_table);
        expr_traverse(t->children[2],var_table);
    }else if(t->children[0]->type=="IF"){ 
        test_traverse(t->children[2],var_table);
        statements_traverse(t->children[5],var_table);
        statements_traverse(t->children[9],var_table);
    }else if(t->children[0]->type=="WHILE"){ 
        test_traverse(t->children[2],var_table);
        statements_traverse(t->children[5],var_table);

    }else if(t->children[0]->type=="PRINTLN"){ 
        expr_traverse(t->children[2],var_table);

    }else if(t->children[0]->type=="DELETE"){ 
        expr_traverse(t->children[3],var_table);
    }

}


//***********************************************************************

string current_type, current_proc;
void procedures_type(Tree *t);  
void procedure_type(Tree *t);    
void main_type(Tree *t);  
string params_type(Tree *t);  
string paramlist_type(Tree *t); 
//string type_type(Tree *t, map<string, string> &var_table);
void dcls_type(Tree *t);
string dcl_type(Tree *t); 
void statements_type(Tree *t); 
void statement_type(Tree *t);
void test_type(Tree *t);   
string expr_type(Tree *t);  
string term_type(Tree *t);  
string factor_type(Tree *t); 
void arglist_type(Tree *t, string id, int position);  
string lvalue_type(Tree *t); 

void procedures_type(Tree *t){
    if(t->children[0]->type == "main"){
        current_proc = "wain";
        main_type(t->children[0]);
    }else {
        current_proc = t->children[0]->children[1]->type ;
        procedure_type(t->children[0]);
        procedures_type(t->children[1]);
    }
}

void procedure_type(Tree *t) {
    
    // expr_type(t->children[9]);
    current_proc = t->children[1]->tokens[0];
    dcls_type(t->children[6]);
    statements_type(t->children[7]);
     if(expr_type(t->children[9]) != "int"){
        cerr<<"ERROR 14"<<endl;
        exit(EXIT_FAILURE);
     }
 }
void main_type(Tree *t) {
    string dcl = dcl_type(t->children[5]);
    if(dcl != "int"){
        cerr<<"ERROR 0"<<endl;
        exit(EXIT_FAILURE);
    }
    dcls_type(t->children[8]);
    statements_type(t->children[9]);
    if(expr_type(t->children[11]) != "int"){
        cerr<<"ERROR 14"<<endl;
        exit(EXIT_FAILURE);
    }
     //current_proc = t->children[1]->tokens[0];

}

string expr_type(Tree *t){
    if(t->children[0]->type=="term"){ 
        return term_type(t->children[0]);
    }else if(t->children[1]->type=="PLUS"){ 
        string expr = expr_type(t->children[0]);
        string term = term_type(t->children[2]);
        if(expr == "int" && term == "int"){
            return "int";
        }else if(expr == "int*" && term =="int"){
            return "int*";
        }else if(expr == "int" && term == "int*"){
            return "int*";
        }else{
            cerr<<"ERROR 1"<<endl;
            exit(EXIT_FAILURE);
        }
    }else {// -
        string expr = expr_type(t->children[0]);
        string term = term_type(t->children[2]);
        if(expr == "int" && term == "int"){
            return "int";
        }else if(expr == "int*" && term =="int"){
            return "int*";
        }else if(expr == "int*" && term == "int*"){
            return "int";
        }else{
            cerr<<"ERROR 2"<<endl;
            exit(EXIT_FAILURE);
        }
    }
}

string term_type(Tree *t){
    if(t->children[0]->type=="factor"){
         return factor_type(t->children[0]);
    }else{
        string term = term_type(t->children[0]);
        string factor = factor_type(t->children[2]);
        if(term =="int" && factor == "int"){
            return "int";
        }else{
            cerr<<"ERROR 3"<<endl;
            exit(EXIT_FAILURE); 
        }
    }
}

string factor_type(Tree *t) {
    if(t->children[0]->type=="ID"){ 
        if(t->children.size()==1){
            return topsymtbl[current_proc].second[t->children[0]->tokens[0]].first;
        }else if (t->children.size()==3){
            if( topsymtbl[t->children[0]->tokens[0]].first.size() != 0){
	       cerr<<"ERROR 9"<<endl;
               exit(EXIT_FAILURE);
            } 
            return "int";
        }else{
            if( topsymtbl[t->children[0]->tokens[0]].first.size() == 0){
	       cerr<<"ERROR 10"<<endl;
               exit(EXIT_FAILURE);
            } 
	    arglist_type(t->children[2],t->children[0]->tokens[0],0);
            return "int";
        }

    }else if(t->children[0]->type=="LPAREN"){ 
        return expr_type(t->children[1]);

    }else if(t->children[0]->type=="AMP"){ 
        string lvalue = lvalue_type(t->children[1]);
        //string lvalue = "int";
        if( lvalue == "int") {
            return "int*";
        }else {
            cerr<<"ERROR 4"<<endl; 
            exit(EXIT_FAILURE);
        }

    }else if(t->children[0]->type=="STAR"){ 
        string factor = factor_type(t->children[1]);
        if(factor == "int*"){
            return "int";
        }else{
            cerr<<"ERROR 5"<<endl; 
            exit(EXIT_FAILURE);
        }
    }else if(t->children[0]->type=="NEW"){ 
        string expr = expr_type(t->children[3]);

        if(expr == "int"){
            return "int*";
        }else{
            cerr<<"ERROR 6"<<endl; 
            exit(EXIT_FAILURE);
        }
    }else if(t->children[0]->type=="NUM"){ 
        return "int"; 
    }else if(t->children[0]->type=="NULL"){ 
        return "int*";
    }
}

string lvalue_type(Tree *t){
    if(t->children[0]->type == "ID"){
        return topsymtbl[current_proc].second[t->children[0]->tokens[0]].first;
    }else if(t->children[0]->type == "STAR"){
        string factor = factor_type(t->children[1]);
        if(factor == "int*"){
            return "int";
        }else{
            cerr<<"ERROR 7"<<endl; 
            exit(EXIT_FAILURE);
        }
    }else if(t->children[0]->type == "LPAREN"){
        return lvalue_type(t->children[1]);
    }

}

string dcl_type(Tree *t) {
    string type;
    if(t->children[0]->children.size()==2){
        type = "int*";
        //cerr <<id<<" int*"<<endl;
    } else {
        type = "int";
        //cerr <<id <<" int"<<endl;
    }
    return type;
}

void dcls_type(Tree *t){ 
    if(t->children.size() != 0 ){
        dcls_type(t->children[0]);
        string dcl = dcl_type(t->children[1]);
        string type = t->children[3]->type;
        if(dcl == "int" && type == "NUM"){
           return;
        }else if(dcl == "int*" && type == "NULL"){
            return;
        }else{
            cerr<<"ERROR 8"<<endl; 
            exit(EXIT_FAILURE);
        }
    }
}

void statements_type(Tree *t){
    if(t->children.size()==0){
        return;
    }else {
        statements_type(t->children[0]);
        statement_type(t->children[1]);
    }
}

void statement_type(Tree *t){
    if(t->children[0]->type=="lvalue"){
        string lvalue = lvalue_type(t->children[0]);
        string expr = expr_type(t->children[2]);
        if(lvalue != expr){
             cerr<<"ERROR 15"<<endl; 
            exit(EXIT_FAILURE);
        }
    }else if(t->children[0]->type=="IF"){ 
        test_type(t->children[2]);
        statements_type(t->children[5]);
        statements_type(t->children[9]);
    }else if(t->children[0]->type=="WHILE"){ 
        test_type(t->children[2]);
        statements_type(t->children[5]);

    }else if(t->children[0]->type=="PRINTLN"){ 
        string expr = expr_type(t->children[2]);
        if(expr != "int"){
            cerr<<"ERROR 16"<<endl; 
            exit(EXIT_FAILURE);
        }
          
    }else if(t->children[0]->type=="DELETE"){ 
        string expr = expr_type(t->children[3]);
        if(expr != "int*"){
            cerr<<"ERROR 17"<<endl; 
            exit(EXIT_FAILURE);
        }
    }

}

void test_type(Tree *t){
   string expr1 = expr_type(t->children[0]);
   string expr2 = expr_type(t->children[2]);
   if(expr1 != expr2){      
      cerr<<"ERROR 18"<<endl; 
      exit(EXIT_FAILURE);
   }	
}

//f(a,b)
//map<string, pair< vector<string>, map<string, string> > > topsymtbl;
void arglist_type(Tree *t, string id, int position){
   vector<string> par = topsymtbl[id].first;
   map<string, pair<string,int> > tlb =  topsymtbl[id].second;
   if(par.size() <= position){
     cerr<<"ERROR 11"<<endl; 
     exit(EXIT_FAILURE);
   }
   string expr = expr_type(t->children[0]);
   string par_id = par[position];
   string par_type = tlb[par_id].first;
   if(expr != par_type){
     cerr<<"ERROR 12"<<endl; 
     exit(EXIT_FAILURE);
   }
   if(t->children.size() != 1){
      arglist_type(t->children[2], id , position +1 );
   }else{
     if(par.size() != position +1){
       cerr<<"ERROR 13"<<endl; 
       exit(EXIT_FAILURE);
     }
   }
}


//***********************************************************************

void procedures_gen(Tree *t);
void procedure_gen(Tree *t);
void main_gen(Tree *t);
//params
//paramlist
//type
void dcls_gen(Tree *t); // dcls and dcl
void statements_gen(Tree *t);
void statement_gen(Tree *t);
void test_gen(Tree *t);
void expr_gen(Tree *t); 
void term_gen(Tree *t);
void factor_gen(Tree *t);
void lvalue_gen(Tree *t);
//arglist
void pushReg(int r){
    cout << "sw $" << r << ", -4($30) ; push $" << r << endl;
    cout << "sub $30, $30, $4" << endl;
}
void popReg(int r){
    cout << "add $30, $30, $4 ; pop $" << r << endl;
    cout << "lw $"<< r <<", -4($30)" << endl;

}

void procedures_gen(Tree *t){
    if(t->children[0]->type == "main"){
        current_proc = "wain";
        main_gen(t->children[0]);
    }else {
        current_proc = t->children[0]->children[1]->type ;
        procedure_gen(t->children[0]);
        procedures_gen(t->children[1]);
    }
}

void procedure_gen(Tree *t) {
 /*   // expr_type(t->children[9]);
    current_proc = t->children[1]->tokens[0];
    dcls_gen(t->children[6]);
    statements_gen(t->children[7]);
     if(expr_gen(t->children[9]) != "int"){
        cerr<<"ERROR 14"<<endl;
        exit(EXIT_FAILURE);
     }    */
 }
void main_gen(Tree *t) {
    // gen the prologue
    cout << "; wain prologue" << endl;
    cout <<".import print" << endl;
    cout << "lis $4 " << endl;
    cout << ".word 4" << endl;
    cout << "lis $11" << endl;
    cout << ".word 1" << endl;
    cout << endl;

    cout << "; setup frame pointer $29" << endl;
    cout << "sub $29, $30, $4" << endl;

    // push $1 and $2 
    cout << "; setup wain parameters" << endl;
    cout << "sw $1, 0($29) " << endl;
    cout << "sw $2, -4($29)" << endl;
    cout << "sub $30, $30, $4" << endl;
    cout << "sub $30, $30, $4" << endl;


    string dcl = dcl_type(t->children[3]);
    if(dcl == "int"){
        cout << "add $2, $0, $0" << endl;
    } // dcl == "int stat"

    //dcl_gen(t->children[3]);
    //dcl_gen(t->children[5]);
    dcls_gen(t->children[8]);
    statements_gen(t->children[9]);
    expr_gen(t->children[11]);
     //current_proc = t->children[1]->tokens[0];
    
    cout << "; wain epilogue" << endl;
    cout << "add $30, $29, $4  ; update stack pointer" << endl;
    cout << "jr $31" << endl;    

}



void dcls_gen(Tree *t){
    if(t->children.size() == 0 ) return;
    dcls_gen(t->children[0]);
    int Offset = topsymtbl[current_proc].second[t->children[1]->children[1]->tokens[0]].second;
    cout << "lis $3" << endl; 
    cout <<".word "<< t->children[3]->tokens[0] << endl;
    cout <<"sw $3, " << Offset <<"($29)"<<endl;
    cout <<"sub $30, $30, $4" << endl;
    
}


void statements_gen(Tree *t){
    if(t->children.size()==0){
        return;
    }else {
        statements_gen(t->children[0]);
        statement_gen(t->children[1]);
    }
}

void test_gen(Tree *t){
    //test → expr LT expr
    //cout << t->children[1]->type << endl;
    if(t->children[1]->type== "LT"){ 
       // cout << "aaaa  " << endl;
        expr_gen(t->children[0]);
        cout << "add $5, $3, $0"<< endl;
        expr_gen(t->children[2]);
        cout << "slt $3, $5, $3"<< endl;
    }else if(t->children[1]->type== "EQ"){
        expr_gen(t->children[0]);
        cout <<"add $5, $3, $0" << endl;
        expr_gen(t->children[2]);
        cout <<"slt $6, $3, $5" << endl;
        cout <<"slt $7, $5, $3" << endl;
        cout <<"add $3, $6, $7" << endl;
        cout << "sub $3, $11, $3" << endl;

    }else if(t->children[1]->type== "NE"){
        expr_gen(t->children[0]);
        cout <<"add $5, $3, $0" << endl;
        expr_gen(t->children[2]);
        cout <<"slt $6, $3, $5" << endl;
        cout <<"slt $7, $5, $3" << endl;
        cout <<"add $3, $6, $7" << endl;

    }else if(t->children[1]->type== "LE"){
        expr_gen(t->children[0]);
        pushReg(3);
        expr_gen(t->children[2]);
        popReg(5);
        cout << "sub $3, $3, $5" << endl;
        cout << "add $3, $11, $3" << endl;
        cout << "slt $3, $0, $3"<< endl;

    }else if(t->children[1]->type== "GE"){
        expr_gen(t->children[0]);
        pushReg(3);
        expr_gen(t->children[2]);
        popReg(5);
        cout << "sub $3, $3, $5" << endl;
        cout << "sub $3, $3, $11" << endl;
        cout << "slt $3, $3, $0"<< endl;
    }else if(t->children[1]->type== "GT"){
        expr_gen(t->children[0]);
        cout << "add $5, $3, $0"<< endl;
        expr_gen(t->children[2]);
        cout << "slt $3, $3, $5"<< endl;
    }

}
int counter = 0;
void statement_gen(Tree *t){
    if(t->children[0]->type=="lvalue"){
        int Offset = topsymtbl[current_proc].second[t->children[0]->children[0]->tokens[0]].second;
        expr_gen(t->children[2]);
       
        cout <<"sw $3, "<< Offset << "($29)"<< endl;
    }else if(t->children[0]->type=="IF"){
//statement → IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE
        
        
        
        int i = counter ++;
        test_gen(t->children[2]);
        cout << "beq $3, $0, else" << i << endl;
        statements_gen(t->children[5]);
        cout << "beq $0, $0, endif" << i << endl;
        cout << "else" << i << ":" << endl;
        statements_gen(t->children[9]);
        cout << "endif" << i << ":" << endl;
        
        /*
        int i = counter++;

        test_gen(t->children[2]);
        cout << "bne $3, $0, 3" << endl;
        cout << "lis $10" << endl ;
        cout << ".word else" << i << endl;
        cout << "jr $10" << endl;
        statements_gen(t->children[5]);
        cout << "else" << i << ":" << endl;

        statements_gen(t->children[9]);
        cout << "lis $10" << endl;
        cout << ".word endif" << i << endl;
        cout << "jr $10" << endl;
        cout << "endif" << i << ":" << endl;
        */
    }else if(t->children[0]->type=="WHILE"){
        //statement → WHILE LPAREN test RPAREN LBRACE statements RBRACE
        int labelcounter = counter++;
        cout << "loop"<< labelcounter << ":" << endl;
        test_gen(t->children[2]);
        cout << "bne $3, $0, continue" << labelcounter << endl;
        cout << "lis $10" << endl ;
        cout << ".word endloop" << labelcounter << endl;
        cout << "jr $10" << endl;
        cout << "continue" << labelcounter << ":" << endl;
        statements_gen(t->children[5]);
        cout << "lis $10" << endl;
        cout << ".word loop" << labelcounter << endl;
        cout << "jr $10" << endl;
        cout << "endloop" << labelcounter << ":" << endl;
        
    }else if(t->children[0]->type=="PRINTLN"){
        expr_gen(t->children[2]);
        cout <<"add $1, $3, $0" << endl;
        pushReg(31);
        pushReg(9);
        cout << "lis $9"<<endl<<".word print"<<endl<<"jalr $9"<<endl;
        popReg(9);
        popReg(31);
    }else if(t->children[0]->type=="DELETE"){

    }
}


void expr_gen(Tree *t){
    if(t->children[0]->type=="term"){
        //cout << " nima 2 " <<endl;
        term_gen(t->children[0]);
        return;
    }
    expr_gen(t->children[0]);
    // push $3 since $3 <- expr
    pushReg(3);
    // $3 <- term
    term_gen(t->children[2]);
    popReg(8);

    if(t->children[1]->type=="PLUS"){
        //expr -> expr PLUS term
        /*
        if(expr=="int*" && term=="int"){
       
        }else if(expr=="int" && term=="int*"){
           
        }

        */
        cout << "add $3, $8, $3" << endl;
    }else if(t->children[1]->type=="MINUS"){
        cout << "sub $3, $8, $3" << endl;
    }
}

void term_gen(Tree *t){
    if(t->children[0]->type=="factor"){
        //cout << "nima1" << endl;
        factor_gen(t->children[0]);
        return;
    }
    term_gen(t->children[0]);
    pushReg(3);
    factor_gen(t->children[2]);
    popReg(8);
    if(t->children[1]->type=="STAR"){
        cout << "mult $8, $3" << endl;
        cout << "mflo $3" << endl;
        return;
    }
    cout << "div $8, $3" << endl;
    if(t->children[1]->type=="SLASH"){
        cout << "mflo $3" << endl;
    }else {
        cout << "mfhi $3" << endl;
    }
}
/*
string factor_type(Tree *t) {
    if(t->children[0]->type=="ID"){ 
        if(t->children.size()==1){
            return topsymtbl[current_proc].second[t->children[0]->tokens[0]];
        }else if (t->children.size()==3){
            if( topsymtbl[t->children[0]->tokens[0]].first.size() != 0){
           cerr<<"ERROR 9"<<endl;
               exit(EXIT_FAILURE);
            } 
            return "int";
        }else{
            if( topsymtbl[t->children[0]->tokens[0]].first.size() == 0){
           cerr<<"ERROR 10"<<endl;
               exit(EXIT_FAILURE);
            } 
        arglist_type(t->children[2],t->children[0]->tokens[0],0);
            return "int";
        }

    }else if(t->children[0]->type=="LPAREN"){ 
        return expr_type(t->children[1]);

    }else if(t->children[0]->type=="AMP"){ 
        string lvalue = lvalue_type(t->children[1]);
        //string lvalue = "int";
        if( lvalue == "int") {
            return "int*";
        }else {
            cerr<<"ERROR 4"<<endl; 
            exit(EXIT_FAILURE);
        }

    }else if(t->children[0]->type=="STAR"){ 
        string factor = factor_type(t->children[1]);
        if(factor == "int*"){
            return "int";
        }else{
            cerr<<"ERROR 5"<<endl; 
            exit(EXIT_FAILURE);
        }
    }else if(t->children[0]->type=="NEW"){ 
        string expr = expr_type(t->children[3]);

        if(expr == "int"){
            return "int*";
        }else{
            cerr<<"ERROR 6"<<endl; 
            exit(EXIT_FAILURE);
        }
    }else if(t->children[0]->type=="NUM"){ 
        return "int"; 
    }else if(t->children[0]->type=="NULL"){ 
        return "int*";
    }
}


void factor_traverse(Tree *t, map<string, string> &var_table) {
    if(t->children[0]->type=="ID"){ 
        if(t->children.size()==1){
            if(var_table.count(t->children[0]->tokens[0]) == 0){
                cerr<<"ERROR"<<endl;
                exit(EXIT_FAILURE);
            }
        }else if (t->children.size()==3){
            if(topsymtbl.count(t->children[0]->tokens[0]) == 0){
                cerr<<"ERROR"<<endl;
                exit(EXIT_FAILURE);
            }
        }else {
            if(topsymtbl.count(t->children[0]->tokens[0]) == 0){
                cerr<<"ERROR"<<endl;
                exit(EXIT_FAILURE);
            }
            arglist_traverse(t->children[2],var_table);


        }
        //cerr<<"ID"<<t->children[0]->tokens[0]<<endl;
    }else if(t->children[0]->type=="LPAREN"){ 
        expr_traverse(t->children[1],var_table);

    }else if(t->children[0]->type=="AMP"){ 
        lvalue_traverse(t->children[1],var_table);

    }else if(t->children[0]->type=="STAR"){ 
        factor_traverse(t->children[1],var_table);
    }else if(t->children[0]->type=="NEW"){ 
        expr_traverse(t->children[3],var_table);

    }else if(t->children[0]->type=="NUM"){ 
        
    }else if(t->children[0]->type=="NULL"){ 

    }
}
*/


void factor_gen(Tree *t){
    //cout <<  "nimaaa  "<< t->children[0]->type<< endl;
    if(t->children[0]->type=="ID"){
        //cout << "nima"<<endl;
        if(t->children.size()==1){
            int Offset = topsymtbl[current_proc].second[t->children[0]->tokens[0]].second;

            cout << "lw $3, " << Offset << "($29)" << endl;
            return;
        }else if (t->children.size()==3){

        }else {

        }
    }else if(t->children[0]->type=="LPAREN"){
        expr_gen(t->children[1]);

    }else if(t->children[0]->type=="NUM"){
        cout << "lis $3" << endl;
        cout << ".word " << t->children[0]->tokens[0] << endl;

    }

}


int main() {
    parse_tree = BuildParseTree();
    procedures_traverse(parse_tree->children[1]);
    procedures_type(parse_tree->children[1]);
    procedures_gen(parse_tree->children[1]);
    return 0;
}

