#include "ast.h"
#include <sstream>

vector<string> constant_data;

void PrintStatement::genConstantData(){
    // struct context context;
    // params->genCode(context);
    // string data = context.code;
    stringstream data;
    data << "print_placeholder_" << constant_data.size() <<  " db " << params << ", 0";
    constant_data.push_back(data.str());
};

string PrintStatement::genCode(){
    stringstream code;
    // struct context context;
    // params->genCode(context);
    code << "\tpush print_placeholder_" << print_id << endl
         << "\tcall printf" << endl
         << "\tadd esp, 4" << endl;

    return code.str();
}