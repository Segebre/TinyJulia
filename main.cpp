
#include <iostream>
#include "ast.h"

extern FILE* yyin;
extern int yyparse();
extern Statement* code_tree;

int main(int argc, char* argv[]){
    if(argc != 2){
        cerr << "Err: Sould be: " << argv[0] << " <input_file.tjl>" << endl;
        exit(1);
    }

    yyin = fopen(argv[1], "r");
    if(yyin == NULL){
        cerr << "Err: Attempt to open file '" << argv[1] << "' failed!" << endl;
        exit(1);
    }
    yyparse();

    if(code_tree == NULL){
        cerr << "Nothing to be compiled." << endl;
        return 0;
    }

    string code = code_tree->genCode();

    cout << "extern printf" << endl
         << "global main" << endl
         << endl << "section .data" << endl;
    for(string data : constant_data){
        cout << data << endl;
    }
    cout << endl << "section .text" << endl
         << "main: " << endl
         << "\tpush ebp" << endl
         << "\tmov ebp, esp" << endl
         << code << endl
         << endl << "\tmov eax, 0" << endl 
         << "\tleave" << endl
         << "\tret" << endl;
         

    return 0;
}