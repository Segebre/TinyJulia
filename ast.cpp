#include "ast.h"
#include <sstream>
#include <map>
#include <stack>
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
    bool is_accessible;
};

struct mini_scope{
    enum{
        NONE,
        IF,
        WHILE,
        FOR
    };
    int type;
    int id;
};

static int global_esp = 0;
static string current_scope = "";
static unsigned int if_count = 0;
static unsigned int while_count = 0;
static unsigned int for_count = 0;
vector<string> constant_data;
stringstream functions;
map<string, struct symbol> global_symbol_table;
map<string, map<string, struct symbol> >local_symbol_table;
map<string, int> local_esp;
map<string, vector<struct function_parameter>*> sanity_check;
map<string, int>function_type;
static struct mini_scope current_mini_scope = {mini_scope::NONE, 0};
stack<vector<string>*> temporal_variables;

int helper_getSize(string name, int type){
    if(current_scope != ""){
        if(local_symbol_table[current_scope].count(name)){
            if(local_symbol_table[current_scope][name].type == TYPE_BOOLEAN && type != TYPE_BOOLEAN){
                std::cerr << "ERR: Variable `" << name << "` does not match type!" << std::endl;
                exit(1);
            }
            return local_symbol_table[current_scope][name].size;
        }
    }
    if(global_symbol_table.count(name)){
        if(global_symbol_table[name].type == TYPE_BOOLEAN && type != TYPE_BOOLEAN){
            std::cerr << "ERR: Variable `" << name << "` does not match type!" << std::endl;
            exit(1);
        }
        return global_symbol_table[name].size;
    }
    else{
        std::cerr << "ERR: Variable `" << name << "` does not exist!" << std::endl;
        exit(1);
    }
}

bool helper_isArray(string name){
    if(!global_symbol_table.count(name)){
        std::cerr << "Variable first use cannot be before its declaration!" << std::endl;
        exit(1);
    }
    if(global_symbol_table[name].size > 1)
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

void ComparisonAndExpression::secondpass(){
    left->secondpass();
    right->secondpass();
    this->type = TYPE_BOOLEAN;
    if(left->getType() != TYPE_BOOLEAN || right->getType() != TYPE_BOOLEAN){
        std::cerr << "non-boolean used in boolean context" << std::endl;
        exit(1);
    }
};

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

void ComparisonOrExpression::secondpass(){
    left->secondpass();
    right->secondpass();
    this->type = TYPE_BOOLEAN;
    if(left->getType() != TYPE_BOOLEAN || right->getType() != TYPE_BOOLEAN){
        std::cerr << "non-boolean used in boolean context" << std::endl;
        exit(1);
    }
};

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
         << "\tpush eax" << " ; " << context_left.comment << " ^ " << context_right.comment;
    
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

void NNExpression::secondpass(){
    value->secondpass();
    if(value->getType() != TYPE_BOOLEAN){
        nnexpression = new NotExpression(value);
        nnexpression->secondpass();
        this->type = nnexpression->getType();
    }
    else{
        nnexpression = new NegExpression(value);
        if(value->getType() != TYPE_BOOLEAN){
            std::cerr << "Err: Cannot negate non-boolean value!" << std::endl;
            exit(1);
        }
        this->type = TYPE_BOOLEAN;
    }
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

void NegExpression::secondpass(){
    value->secondpass();
    if(value->getType() != TYPE_BOOLEAN){
        std::cerr << "Err: Cannot negate non-boolean value!" << std::endl;
        exit(1);
    }
    this->type = TYPE_BOOLEAN;
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

void IdentifierExpression::secondpass(){
        if(current_scope == ""){
            if(!global_symbol_table.count(name) || !global_symbol_table[name].is_accessible){
                std::cerr << "ERR: Variable `" << name << "` was first used before its declaration!" << std::endl;
                exit(1);
            }
            type = global_symbol_table[name].type;
        }
        else{
            if(local_symbol_table[current_scope].count(name) && local_symbol_table[current_scope][name].is_accessible)
                type = local_symbol_table[current_scope][name].type;
            else if(global_symbol_table.count(name) && global_symbol_table[name].is_accessible)
                type = global_symbol_table[name].type;
            else{
                std::cerr << "ERR: Variable `" << name << "` was first used before its declaration!" << std::endl;
                exit(1);
            }
        }

        if(position == NULL){
            this->array = helper_getSize(name, this->type) == 1?false:true;
            this->position = new IntegerExpression(getSize());
        }
        this->position->secondpass();
    }

void IdentifierExpression::genCode(struct context& context){
    stringstream code;
    stringstream comment;
    struct context position_context;

    position->genCode(position_context);
    code << position_context.code << " ; offset ; " << (position_context.is_printable?position_context.comment:"") << endl
        << "\tpop eax" << endl
        << "\tdec eax" << " ; position-1 for indexing from 0" << endl;
    if(current_scope == "")
        code << "\tpush dword [ebp" << (global_symbol_table[name].position >= 0?"+":"") << global_symbol_table[name].position << "+eax*4]";
    else{
        if(local_symbol_table[current_scope].count(name))
            code << "\tpush dword [ebp" << (local_symbol_table[current_scope][name].position >= 0?"+":"") << local_symbol_table[current_scope][name].position << "+eax*4]";
        else if(global_symbol_table.count(name)){
            code << "\tmov ecx, [ebp] ; change of scope" << endl
                 << "\tpush dword [ecx" << (global_symbol_table[name].position >= 0?"+":"") << global_symbol_table[name].position << "+eax*4]";
        }
    }

    comment << name << "[" << position_context.comment << "]";

    context.code = code.str();
    context.comment = comment.str();
    context.is_printable = true;
}

 void FunctionExpression::secondpass(){
    for(Expression* parameter : parameters)
        parameter->secondpass();

    if(sanity_check[name]->size() != parameters.size()){
        std::cerr << "ERR: Expected " << sanity_check[name]->size() << " parameter(s) but found " << parameters.size() << std::endl;
        exit(1);
    }

    for(int index = 0; index < sanity_check[name]->size(); index++){
        if(((*sanity_check[name])[index].type == TYPE_BOOLEAN) && (parameters[index]->getType() != TYPE_BOOLEAN)){
            std::cerr << "ERR: Incompatible types on parameter `" << (*sanity_check[name])[index].name << "`" << std::endl;
            exit(1);
        }
        else if(parameters[index]->getSize() != (*sanity_check[name])[index].size){
            int temp = parameters[index]->getSize();
            temp = (*sanity_check[name])[index].size;
            std::cerr << "ERR: Incompatible sizes on parameter `" << (*sanity_check[name])[index].name << "`, expected " << (*sanity_check[name])[index].size << " found " << parameters[index]->getSize() << std::endl;
            exit(1);
        }
    }

    type = function_type[name];
}

void FunctionExpression::addParameter(Expression* parameter){
    parameters.push_back(parameter);
}

void FunctionExpression::genCode(struct context& context){
    stringstream code;
    stringstream comment;
    int count = parameters.size();
    for(vector<Expression*>::reverse_iterator parameter = parameters.rbegin(); parameter != parameters.rend(); parameter++){
        struct context parameter_context;
        (*parameter)->genCode(parameter_context);
        code << parameter_context.code << " ; Parmeter " << count-- << endl;
    }
    code << "\tcall " << name << endl
         << "\tadd esp, " << parameters.size()*4 << endl
         << "\tpush eax";
        
    comment << " ; " << name << "() return value";

    context.code = code.str();
    context.comment = comment.str();
    context.is_printable = false;
}

string ExpressionStatement::genCode(){
    stringstream code;
    struct context context;
    expression->genCode(context);
    code << context.code << endl
         << "\tpop eax ; Unused result lost" << endl;
    return code.str();
};

void PrintStatement::secondpass(){ 
    for(vector<struct parameter_type>::iterator parameter = parameters.begin(); parameter != parameters.end(); parameter++)
        if(parameter->type == -1){
            parameter->expression->secondpass();
            parameter->type = parameter->expression->getType();
        }
    this->genConstantData();
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
            if(parameter->expression->getSize() > 1){
                std::cerr << "ERR: `" << ((IdentifierExpression*)parameter->expression)->getName() << "`: Arrays are not allowed in Print Statement!" << std::endl;
                exit(1);
            }
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

void FunctionStatement::secondpass(){
    if(current_scope != ""){
        std::cerr << "Function declaration inside a function not allowed!" << std::endl;
        exit(1);
    }
    if(global_symbol_table.count(name)){
        std::cerr << "Variable with this name already exists!" << std::endl;
        exit(1);
    }
    if(local_symbol_table.count(name)){
        std::cerr << "Function redeclaration not allowed!" << std::endl;
        exit(1);
    }

    sanity_check[name] = function_params;
    function_type[name] = type;

    int offset = 8;
    for(function_parameter fp : *function_params){
        if(local_symbol_table[name].count(fp.name)){
            std::cerr << "Err: Found repeated parameter names on function declaration!" << std::endl;
            exit(1);
        }
        struct symbol symbol;
        symbol.type = fp.type;
        symbol.position = offset;
        symbol.size = fp.size;
        offset += symbol.size*4;
        local_symbol_table[name][fp.name] = symbol;
    }

    current_scope = name;
    body->secondpass();
    current_scope = "";
}

string FunctionStatement::genCode(){
    current_scope = name;
    functions << name << ":" << endl
        << "\tpush ebp" << endl
        << "\tmov ebp, esp" << endl
        << body->genCode() << endl
        << "\tleave" << endl
        << "\tret" << endl;
    current_scope = "";
    return string("");
}

void ReturnStatement::secondpass(){
    value->secondpass();
    if(current_scope != "")
        if(function_type[current_scope] == TYPE_BOOLEAN && value->getType() != TYPE_BOOLEAN){
            std::cerr << "Err: Return type mismatch for function `" << current_scope << "`!" << std::endl;
            exit(1);
        }
}

string ReturnStatement::genCode(){
    stringstream code;
    struct context context;
    value->genCode(context);

    if(current_scope == ""){
        switch(current_mini_scope.type){
            case mini_scope::NONE: code << "" << endl; break;
            case mini_scope::IF: code << "\tjmp if_end_" << current_mini_scope.id << endl; break;
            case mini_scope::WHILE: code << "\tjmp while_end_" << current_mini_scope.id << endl; break;
            case mini_scope::FOR: code << "\tjmp for_end_" << current_mini_scope.id << endl; break;
        }
    }
    else
        code << context.code << endl
             << "\tpop eax, " << " ; " << context.comment << endl
             << "\tleave" << endl
             << "\tret" << endl;

    return code.str();
}

void IfStatement::secondpass(){
    if_id = if_count++;
    bool reset = false;
    if(current_mini_scope.type == mini_scope::NONE){
        current_mini_scope.type = mini_scope::IF;
        current_mini_scope.id = if_id;
        reset = true;
    }
    
    condition->secondpass();
    trueBlock->secondpass();
    falseBlock->secondpass();
    
    if(reset){
        current_mini_scope.type = mini_scope::NONE;
        current_mini_scope.id = -1;
    }
}

string IfStatement::genCode(){
    stringstream code;
    struct context condition_context;
    bool reset = false;
    if(current_mini_scope.type == mini_scope::NONE){
        current_mini_scope.type = mini_scope::IF;
        current_mini_scope.id = if_id;
        reset = true;
    }

    condition->genCode(condition_context);

    code << "if_start_" << if_id << ":" << endl
         << condition_context.code << " ; Condition" << (condition_context.is_printable?("is " + condition_context.comment):"") << endl
         << "\tpop eax" << endl
         << "\tcmp eax, 0" << endl
         << "\tje if_else_" << if_id << endl
         << trueBlock->genCode() << endl
         << "\tjmp if_end_" << if_id << endl
         << "if_else_" << if_id << ":" << endl
         << falseBlock->genCode() << endl
         << "if_end_" << if_id << ":" << endl;
        
    if(reset){
        current_mini_scope.type = mini_scope::NONE;
        current_mini_scope.id = -1;
    }
    
    return code.str();
}

void WhileStatement::secondpass(){
    this->while_id = while_count++;
    bool reset = false;
    if(current_mini_scope.type == current_mini_scope.NONE){
        current_mini_scope.id = while_id;
        current_mini_scope.type = current_mini_scope.WHILE;
        reset = true;
    }

    temporal_variables.push(new vector<string>);

    condition->secondpass();
    trueBlock->secondpass();

    for(string temporal_variable : *temporal_variables.top()){
        if(current_scope != "" && local_symbol_table[current_scope].count(temporal_variable)){
            local_symbol_table[current_scope][temporal_variable].is_accessible = false;
            local_esp[current_scope] += local_symbol_table[current_scope][temporal_variable].size;
        }
        else{
            global_symbol_table[temporal_variable].is_accessible = false;
            global_esp += global_symbol_table[temporal_variable].size*4;
        }
    }
    delete temporal_variables.top();
    temporal_variables.pop();

    if(reset){
        current_mini_scope.id = -1;
        current_mini_scope.type = current_mini_scope.NONE;
    }
}

string WhileStatement::genCode(){
    stringstream code;
    struct context context;
    bool reset = false;

    if(current_mini_scope.type == current_mini_scope.NONE){
        current_mini_scope.type = current_mini_scope.WHILE;
        current_mini_scope.id = while_id;
    }

    condition->genCode(context);

    code << "\tpush ebx" << endl
         << "\tmov ebx, esp" << endl
         << "while_start_" << while_id << ":" << endl
         << context.code << " ; condition ; " << context.comment << endl
         << "\tpop eax" << endl
         << "\tcmp eax, 0" << endl
         << "\tje while_end_" << while_id << endl
         << trueBlock->genCode() << endl
         << "\tjmp while_start_" << while_id << endl
         << "while_end_" << while_id << ":" << endl
         << "\tmov esp, ebx" << endl
         << "\tpop ebx" << endl;

    if(reset){
        current_mini_scope.type = current_mini_scope.NONE;
        current_mini_scope.id = -1;
    }

    return code.str();
}

void ForStatement::secondpass(){
    this->for_id = for_count++;
    bool reset = false;

    if(current_mini_scope.type == current_mini_scope.NONE){
        current_mini_scope.type = current_mini_scope.FOR;
        current_mini_scope.id = for_count;
        reset = true;
    }

    temporal_variables.push(new vector<string>);
    
    from->secondpass();
    to->secondpass();

    if((current_scope != "" && !local_symbol_table[current_scope].count(name)) || !global_symbol_table.count(name))
        create_variable = new DeclareStatement(name, TYPE_INTEGER, 1);
    else
        create_variable = new StatementBlock();

    create_variable->secondpass();
    trueBlock->secondpass();

    for(string temporal_variable : *temporal_variables.top()){
        if(current_scope != "" && local_symbol_table[current_scope].count(temporal_variable)){
            local_symbol_table[current_scope][temporal_variable].is_accessible = false;
            local_esp[current_scope] += local_symbol_table[current_scope][temporal_variable].size*4;
        }
        else if(global_symbol_table.count(temporal_variable)){
            global_symbol_table[temporal_variable].is_accessible = false;
            global_esp -= global_symbol_table[temporal_variable].size*4;
        }
    }
    delete temporal_variables.top();
    temporal_variables.pop();

    if(reset){
        current_mini_scope.type = current_mini_scope.NONE;
        current_mini_scope.id = -1;
    }
}

string ForStatement::genCode(){
    stringstream code;
    struct context from_context;
    struct context to_context;

    from->genCode(from_context);
    to->genCode(to_context);

    code << "\tpush ebx" << endl
         << "\tmov ebx, esp" << endl
         << from_context.code << " ; from ; " << from_context.comment << endl
         << to_context.code << " ; to ; " << to_context.comment << endl
         << create_variable->genCode() << endl
         << "for_start_" << for_id << ":" << endl
         << "\tmov eax, [ebx-4]" << endl
         << "\tcmp eax, [ebx-8]" << endl
         << "\tjg for_end_" << for_id << endl
         << "\tpush dword [ebx-4]" << endl;
    if(current_scope == ""){
        code << "\tpop dword [ebp" << (global_symbol_table[name].position>=0?"+":"") << global_symbol_table[name].position << "]" << endl;
    }
    else{
        if(local_symbol_table[current_scope].count(name))
            code << "\tpop dword [ebp" << (local_symbol_table[current_scope][name].position>=0?"+":"") << local_symbol_table[current_scope][name].position << "]" << endl;
        else{
            code << "\tmov eax, [ebp]" << endl
                 << "\tpop dword [eax" << (global_symbol_table[name].position>=0?"+":"") << global_symbol_table[name].position << "]" << endl;
        }
    }
    code << trueBlock->genCode() << endl
         << "\tinc dword [ebx-4]" << endl
         << "\tjmp for_start_" << for_id << endl
         << "for_end_" << for_id << ":" << endl
         << "\tmov esp, ebx" << endl
         << "\tpop ebx" << endl;


    return code.str();
}

void DeclareStatement::secondpass(){
    struct symbol symbol;
    symbol.type = type;
    symbol.size = size;
    symbol.is_accessible = true;
    
    if(size < 1){
        std::cerr << "ERR: Size of Array cannot be less than 1!" << std::endl;
        exit(1);
    }
    if(current_scope == ""){
        if(global_symbol_table.count(name) && global_symbol_table[name].is_accessible){
            std::cerr << "ERR: Variable redeclaration not allowed!" << std::endl;
            exit(1);
        }
        symbol.position = global_esp-size*4;
        global_esp = symbol.position;
        global_symbol_table[name] = symbol;
    }
    else{
        if(local_symbol_table[current_scope].count(name) && local_symbol_table[current_scope][name].is_accessible){
            std::cerr << "ERR: Variable redeclaration not allowed!" << std::endl;
            exit(1);
        }
        else if(global_symbol_table.count(name) && global_symbol_table[name].is_accessible){
            std::cerr << "ERR: Variable redeclaration not allowed!" << std::endl;
            exit(1);
        }
        symbol.position = local_esp[current_scope]-size*4;
        local_esp[current_scope] = symbol.position;
        local_symbol_table[current_scope][name] = symbol;
    }

    if(!temporal_variables.empty())
        temporal_variables.top()->push_back(name);
}

string DeclareStatement::genCode(){
    stringstream code;
    code << "\tsub esp, " << size*4 << " ; Declaration of `" << name << "`";
    return code.str();
}

void SetStatement::secondpass(){
    expression->secondpass();
    position->secondpass();
    if(current_scope == ""){
        if(!global_symbol_table.count(name) || !global_symbol_table[name].is_accessible){
            std::cerr << "ERR: Variable `" << name << "` was first used before its declaration!" << std::endl;
            exit(1);
        }
        else if(global_symbol_table[name].type == TYPE_BOOLEAN && expression->getType() != TYPE_BOOLEAN){
            std::cerr << "ERR: Incompatible types!" << std::endl;
            exit(1);
        }
    }
    else{
        if(!local_symbol_table[current_scope].count(name) || !local_symbol_table[current_scope][name].is_accessible){
            if(!global_symbol_table.count(name)){
                std::cerr << "ERR: Variable `" << name << "` was first used before its declaration!" << std::endl;
                exit(1);
            }
            else if(global_symbol_table[name].type == TYPE_BOOLEAN && expression->getType() != TYPE_BOOLEAN){
                std::cerr << "ERR: Incompatible types!" << std::endl;
                exit(1);
            }
        }
        else if(local_symbol_table[current_scope][name].type == TYPE_BOOLEAN && expression->getType() != TYPE_BOOLEAN){
            std::cerr << "ERR: Incompatible types!" << std::endl;
            exit(1);
        }
    }
}

string SetStatement::genCode(){
    stringstream code;
    struct context expression_context;
    struct context position_context;

    expression->genCode(expression_context);
    position->genCode(position_context);

    code << position_context.code << endl
         << expression_context.code << endl
         << "\tpop eax" << " ; value ; " << expression_context.comment << endl
         << "\tpop ecx" << " ; offset ; " << position_context.comment << endl
         << "\tdec ecx" << " ; position-1 for indexing from 0" << endl;
    
    if(current_scope == ""){
            code << "\tmov [ebp" << (global_symbol_table[name].position >= 0?"+":"") << global_symbol_table[name].position << "+ecx*4], eax ; "
                << name << "[" << position_context.comment << "]" << "=" << expression_context.comment << endl;
    }
    else{
        if(!local_symbol_table[current_scope].count(name)){
            code << "\tmov edx, [ebp] ; scope change" << endl
                << "\tmov [edx" << (global_symbol_table[name].position >= 0?"+":"") << global_symbol_table[name].position << "+ecx*4], eax ; "
                << name << "[" << position_context.comment << "]" << "=" << expression_context.comment << endl;
        }
        else
            code << "\tmov [ebp" << (local_symbol_table[current_scope][name].position >= 0?"+":"") << local_symbol_table[current_scope][name].position << "+ecx*4], eax ; "
                << name << "[" << position_context.comment << "]" << "=" << expression_context.comment << endl;
    }
    return code.str();
}

void AssignStatement::secondpass(){
    position->secondpass();
    rightside->secondpass();

    switch(variant){
        case 1:{
            if(rightside->getSize() > 1){
                std::cerr << "ERR: Incompatible sizes on `" << name << "` assignation!" << std::endl;
                exit(1);
            }
            statement = new StatementBlock();
            ((StatementBlock*)statement)->addStatement(new DeclareStatement(name, type, 1));
            ((StatementBlock*)statement)->addStatement(new SetStatement(name, rightside, position));
            statement->secondpass();
            break;
        }
        case 2:{
            if(rightside->getSize() > 1){
                std::cerr << "ERR: Incompatible sizes on `" << name << "` assignation!" << std::endl;
                exit(1);
            }
            statement = new SetStatement(name, rightside, position);
            statement->secondpass();
            break;
        }
        case 3:{
            int size = rightside->getSize();
            if(size != helper_getSize(name, TYPE_BOOLEAN)){
                std::cerr << "ERR: Incompatible sizes on `" << name << "` assignation!" << std::endl;
                exit(1);
            }
            if(size == 1)
                statement = new SetStatement(name, rightside, position);
            else{
                statement = new StatementBlock();
                for(int index = 1; index <= size; index++)
                    ((StatementBlock*)statement)->addStatement(new SetStatement(name, new IdentifierExpression(((IdentifierExpression*)rightside)->getName(), new IntegerExpression(index)), new IntegerExpression(index)));
            }
            statement->secondpass();
            break;
        }
        case 4:{
            int size = helper_getSize(((IdentifierExpression*)rightside)->getName(), type);
            statement = new StatementBlock();
            ((StatementBlock*)statement)->addStatement(new DeclareStatement(name, type, size));
            for(int pos = 1; pos <= size; pos++)
                ((StatementBlock*)statement)->addStatement(new SetStatement(name, new IdentifierExpression(((IdentifierExpression*)rightside)->getName(), new IntegerExpression(pos)), new IntegerExpression(pos))); 
            statement->secondpass();
            break;
        }
    }
}