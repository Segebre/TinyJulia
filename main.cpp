
#include <iostream>
#include "ast.h"

extern FILE* yyin;
extern int yyparse();
extern vector<string> constant_data;
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

    code_tree->secondpass();
    string code = code_tree->genCode();

    cout << "; Code generated by TinyPython ;" << endl
         << "extern printf" << endl
         << "global main" << endl
         << endl << "section .data" << endl
         << "print_placeholder_true db \"true\", 0" << endl
         << "print_placeholder_false db \"false\", 0" << endl;
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
         << "\tret" << endl
         << endl << endl << endl << ";----Tiny Julia Helper Code -----;" << endl
         << "TinyJulia_interpret_bool:" << endl
         << "\tcmp eax, 0" << endl
         << "\tje TinyJulia_interpret_bool_set_false" << endl
         << "\tmov eax, print_placeholder_true" << endl
         << "\tret" << endl
         << "TinyJulia_interpret_bool_set_false:" << endl
         << "\tmov eax, print_placeholder_false" << endl
         << "\tret" << endl
         << endl << endl
         << "TinyJulia_exponenciation:" << endl
         << "\tpush ebp" << endl
         << "\tmov ebp, esp" << endl
         << "\tmov ecx, [ebp+8]" << endl
         << "\tmov eax, 1" << endl
         << "\tcdq" << endl
         << "TinyJulia_exponenciation_loop:" << endl
         << "\tcmp dword [ebp+12], 0" << endl
         << "\tjng TinyJulia_exponenciation_loop_end" << endl
         << "\timul ecx" << endl
         << "\tdec dword [ebp+12]" << endl
         << "\tjmp TinyJulia_exponenciation_loop" << endl
         << "TinyJulia_exponenciation_loop_end:" << endl
         << "\tleave" << endl
         << "\tret" << endl;
         

    return 0;
}