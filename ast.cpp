#include "ast.h"
#include <sstream>
#include <map>
#include <iostream>

#define COMPARISON(name)                                            \
    void name##Expression::genCode(struct context& context){        \
    struct context context_right;                                   \
    struct context context_left;                                    \
                                                                    \
    right->genCode(context_right);                                  \
    left->genCode(context_left);                                    \
                                                                    \
    stringstream code;                                              \
    code << context_left.code << endl                               \
         << context_right.code << endl                              \
         << "\tcall TinyJulia_" << #name << "_comparison" << endl   \
         << "\tadd esp, 8" << endl                                  \
         << "\tpush eax";                                           \
                                                                    \
    context.code = code.str();                                      \
    context.comment = #name;                                        \
    context.comment += " comparison result";                        \
    context.is_printable = false;                                   \
}

struct symbol{
    int type;
    int position;
    int size;
};

vector<string> constant_data;
map<string, struct symbol> symbol_table;
static int esp = 0;

void helper_DeclareVariable(string name, int type, int size){
    if(symbol_table.count(name)){
        std::cerr << "ERR: Variable redeclaration not allowed!" << std::endl;
        exit(1);
    }
    struct symbol symbol;
    symbol.type = type;
    symbol.position = esp-size*4;
    symbol.size = size;
    esp -= symbol.position;
    symbol_table[name] = symbol;
}

void helper_SetVariable(string name, int type, Expression* position){
    if(!symbol_table.count(name)){
        std::cerr << "ERR: Variable `" << name << "` was first used before its declaration!" << std::endl;
        exit(1);
    }
    if(symbol_table[name].type == TYPE_BOOLEAN && type != TYPE_BOOLEAN){
        std::cerr << "ERR: Incompatible types!" << std::endl;
        exit(1);
    }
}

int helper_UseVariable(string name){
    if(!symbol_table.count(name)){
        std::cerr << "ERR: Variable `" << name << "` was first used before its declaration!" << std::endl;
        exit(1);
    }
    return symbol_table[name].type;
}

bool helper_isArray(string name){
    if(!symbol_table.count(name)){
        std::cerr << "Variable first use cannot be before its declaration!" << std::endl;
        exit(1);
    }
    if(symbol_table[name].size > 1)
        return true;
    return false;
}

int helper_DeciferType(Expression* left, Expression* right){
    if(left->getType() == TYPE_BOOLEAN && right->getType() == TYPE_BOOLEAN)
        return TYPE_BOOLEAN;
    return TYPE_INTEGER;
}

COMPARISON(GT)
COMPARISON(LT)
COMPARISON(EQ)
COMPARISON(GE)
COMPARISON(LE)
COMPARISON(NE)

void ComparisonAndExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tcall TinyJulia_comparison_and" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Comparison AND result";
    context.is_printable = false;
}

void ComparisonOrExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tcall TinyJulia_comparison_or" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Comparison OR result";
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

void SalExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tsal eax, cl" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Shift Left result";
    context.is_printable = false;
}

void SarExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tsar eax, cl" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Shift Arithmetic Right result";
    context.is_printable = false;
}

void SlrExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tshr eax, cl" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Shift Logical Right result";
    context.is_printable = false;
}

void OrExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tor eax, ecx" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Logical OR result";
    context.is_printable = false;
}

void XorExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\txor eax, ecx" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Logical XOR result";
    context.is_printable = false;
}

void AndExpression::genCode(struct context& context){
    struct context context_right;
    struct context context_left;

    right->genCode(context_right);
    left->genCode(context_left);
    
    stringstream code;
    code << context_left.code << endl
         << context_right.code << endl
         << "\tpop ecx" << " ; " << (context_right.is_printable?context_right.comment:"") << endl
         << "\tpop eax" << " ; " << (context_left.is_printable?context_left.comment:"") << endl
         << "\tand eax, ecx" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "Logical AND result";
    context.is_printable = false;
}

void NotExpression::genCode(struct context& context){
    value->genCode(context);
    stringstream code;
    code << context.code << endl
         << "\tpop eax" << " ; " << (context.is_printable?context.comment:"") << endl
         << "\nnot eax" << endl
         << "\tpush eax";

    context.code = code.str();
    context.comment = "NOT result";
    context.is_printable = false;
}

void NegExpression::genCode(struct context& context){
    value->genCode(context);
    stringstream code;
    code << context.code << endl
         << "\tpop eax" << " ; " << (context.is_printable?context.comment:"") << endl
         << "\tcall TinyJulia_negate_value" << endl
         << "\tpush eax" << endl;

    context.code = code.str();
    context.comment = "NEG result";
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

void IdentifierExpression::genCode(struct context& context){
    stringstream code;
    struct context position_context;

    position->genCode(position_context);
    code << position_context.code << " ; " << position_context.comment << endl
         << "\tpop eax" << endl
         << "\tdec eax" << " ; position-1 for indexing from 0" << endl
         << "\tpush dword [ebp" << (symbol_table[name].position >= 0?"+":"") << symbol_table[name].position << "+eax*4]";

    context.code = code.str();
    context.comment = name;
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

string DeclareStatement::genCode(){
    stringstream code;
    code << "\tsub esp, " << size*4 << " ; Declaration of `" << name << "`";
    return code.str();
}

string SetStatement::genCode(){
    stringstream code;
    struct context expression_context;
    struct context position_context;

    expression->genCode(expression_context);
    position->genCode(position_context);

    code << position_context.code << endl
         << expression_context.code << endl
         << "\tpop eax" << " ; " << expression_context.comment << endl
         << "\tpop ecx" << " ; " << position_context.comment << endl
         << "\tdec ecx" << " ; position-1 for indexing from 0" << endl
         << "\tmov [ebp" << (symbol_table[name].position>0?"+":"") << symbol_table[name].position << "+ecx*4], eax" << endl;

    return code.str();
}