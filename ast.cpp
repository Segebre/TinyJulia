#include "ast.h"
#include <sstream>

vector<string> constant_data;

void GTExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tcall TinyJulia_GT_comparison" << endl
         << "\tadd esp, 8" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Addition result";
    context.is_printable = false;
}

void AddExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tadd eax, ecx" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Addition result";
    context.is_printable = false;
}

void SubExpression::genCode(struct context& context){
    struct context context_left;
    struct context context_right;

    left->genCode(context_left);
    right->genCode(context_right);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tsub eax, ecx" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Subtraction result";
    context.is_printable = false;
}

void MulExpression::genCode(struct context& context){
    struct context context_left;
    struct context context_right;

    left->genCode(context_left);
    right->genCode(context_right);

    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tcdq" << endl
         << "\timul ecx" << " ; " << context_left.comment << " * " << context_right.comment << endl
         << "\tpush eax";
    
    context.code = code.str();
    context.comment = "Multiplication result";
    context.is_printable = false;
}

void DivExpression::genCode(struct context& context){
    struct context context_left;
    struct context context_right;

    left->genCode(context_left);
    right->genCode(context_right);

    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tcdq" << endl
         << "\tidiv ecx" << " ; " << context_left.comment << " / " << context_right.comment << endl
         << "\tpush eax";
    
    context.code = code.str();
    context.comment = "Divition result";
    context.is_printable = false;
}

void ModExpression::genCode(struct context& context){
    struct context context_left;
    struct context context_right;

    left->genCode(context_left);
    right->genCode(context_right);

    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tcdq" << endl
         << "\tidiv ecx" << " ; " << context_left.comment << " % " << context_right.comment << endl
         << "\tpush edx";
    
    context.code = code.str();
    context.comment = "Module result";
    context.is_printable = false;
}

void PowExpression::genCode(struct context& context){
    struct context context_left;
    struct context context_right;

    left->genCode(context_left);
    right->genCode(context_right);

    stringstream code;
    code << context_right.code << endl
         << context_left.code << endl
         << "\tcall TinyJulia_exponenciation" << endl
         << "\tadd esp, 8" << endl
         << "\tpush eax" << " ; " << context_left.comment << " ^ " << context_right.comment << endl;
    
    context.code = code.str();
    context.comment = "Exponentiation result";
    context.is_printable = false;
}

void IntegerExpression::genCode(struct context& context){
    stringstream code;
    code << "\tpush dword " << this->integer;

    context.code = code.str();
    context.comment = to_string(this->integer);
    context.is_printable = true;
}

void BooleanExpression::genCode(struct context& context){
    stringstream code;
    code << "\tpush dword " << this->boolean;

    context.code = code.str();
    context.comment = this->boolean?"true":"false";
    context.is_printable = true;
}

void PrintStatement::genConstantData(){
    print_id = constant_data.size();
    
    stringstream data;
    data << "print_placeholder_" << print_id <<  " db ";
    for(vector<struct parameter_type>::iterator parameter = parameters.begin(); parameter != parameters.end();){
        if(parameter->type == TYPE_LITERAL){
            data << *parameter->literal;
            delete parameter->literal;
        }
        else if(parameter->type == TYPE_INTEGER)
            data << "\"%d\"";
        else if(parameter->type == TYPE_BOOLEAN)
            data << "\"%s\"";

        if(++parameter != parameters.end())
            data << ", ";
    }
    data << (isprintline?", 10":"") << ", 0";
    constant_data.push_back(data.str());
};

string PrintStatement::genCode(){
    int stackLeveling = 4;
    stringstream code;
    for(vector<struct parameter_type>::reverse_iterator parameter = parameters.rbegin(); parameter != parameters.rend(); parameter++){
        if(parameter->type == TYPE_LITERAL){
            continue;
        }
        else if(parameter->type == TYPE_INTEGER || parameter->type == TYPE_BOOLEAN){
            struct context context;
            parameter->expression->genCode(context);
            code << context.code;
            if(parameter->type == TYPE_BOOLEAN){
                code << endl 
                     << "\tpop eax" << endl
                     << "\tcall TinyJulia_interpret_bool" << endl
                     << "\tpush eax" << endl;
            }
            if(context.is_printable)
                code << " ; " << context.comment;
            code << endl;
            stackLeveling+=4;
        }
    }

    code << "\tpush print_placeholder_" << print_id << endl
         << "\tcall printf" << endl
         << "\tadd esp, " << stackLeveling << endl;

    return code.str();
}