#include "ast.h"
#include <sstream>

vector<string> constant_data;

void PrintStatement::genConstantData(){
    stringstream data;
    data << "print_placeholder_" << constant_data.size() <<  " db ";
    for(vector<struct parameter_type>::iterator parameter = parameters.begin(); parameter != parameters.end();){
        data << *parameter->literal;
        if(++parameter != parameters.end())
            data << ", ";
    }
    data << (isprintline?", 10":"") << ", 0";
    constant_data.push_back(data.str());
};

string PrintStatement::genCode(){
    stringstream code;
    code << "\tpush print_placeholder_" << print_id << endl
         << "\tcall printf" << endl
         << "\tadd esp, 4" << endl;

    return code.str();
}

//cambiar el extern