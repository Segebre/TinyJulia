#include "ast.h"
#include <sstream>

vector<string> constant_data;

void PrintStatement::genConstantData(){
    print_id = constant_data.size();
    
    stringstream data;
    data << "print_placeholder_" << print_id <<  " db ";
    for(vector<struct parameter_type>::iterator parameter = parameters.begin(); parameter != parameters.end();){
        if(parameter->type == TYPE_LITERAL){
            data << *parameter->literal;
            delete parameter->literal;
        }
        else if(parameter->type == TYPE_NUMBER)
            data << "\"" << parameter->number << "\"";

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