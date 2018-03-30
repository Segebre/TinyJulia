#ifndef AST_H
#define AST_H

#define BINARYEXPRESSIONHELPER(name, valuetype)                                                                             \
    class name##Expression : public BinaryExpression{                                                                       \
    public:                                                                                                                 \
        name##Expression(Expression* left, Expression* right) : BinaryExpression(left, right){}                             \
        void secondpass(){ left->secondpass(); right->secondpass(); this->type = valuetype; };                                 \
        void genCode(struct context& context);                                                                              \
    };


#include <string>
#include <sstream>
#include <vector>

using namespace std;

enum{
    TYPE_LITERAL,
    TYPE_INTEGER,
    TYPE_BOOLEAN
};

struct function_parameter{
    string name;
    int type;
    int size;
};

class AST{
public:
    virtual void secondpass() = 0;
};

/////////////////
// Expressions //
/////////////////

class Expression : public AST{
public:
    virtual void genCode(struct context& context) = 0;
    int getType(){ return this->type; }
    virtual int getSize(){ return 1; }
protected:
    int type;
};

extern void helper_SetVariable(string name, int type, Expression* position);
extern int helper_DeciferType(Expression* left, Expression* right);
extern int helper_getSize(string name, int type);
extern void helper_resetScope();

class BinaryExpression : public Expression{
public:
    BinaryExpression(Expression* left, Expression* right){
        this->left = left;
        this->right = right;
    }
protected:
    Expression* left;
    Expression* right;
};

BINARYEXPRESSIONHELPER(Add, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Sub, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Mul, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Div, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Mod, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Pow, TYPE_INTEGER);

BINARYEXPRESSIONHELPER(GT, TYPE_BOOLEAN);
BINARYEXPRESSIONHELPER(LT, TYPE_BOOLEAN);
BINARYEXPRESSIONHELPER(EQ, TYPE_BOOLEAN);
BINARYEXPRESSIONHELPER(GE, TYPE_BOOLEAN);
BINARYEXPRESSIONHELPER(LE, TYPE_BOOLEAN);
BINARYEXPRESSIONHELPER(NE, TYPE_BOOLEAN);

class ComparisonAndExpression : public BinaryExpression{
public:
    ComparisonAndExpression(Expression* left, Expression* right) : BinaryExpression(left, right){}
    void secondpass();
    void genCode(struct context& context);
};

class ComparisonOrExpression : public BinaryExpression{
public:
    ComparisonOrExpression(Expression* left, Expression* right) : BinaryExpression(left, right){}
    void secondpass();
    void genCode(struct context& context);
};

BINARYEXPRESSIONHELPER(Sal, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Sar, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Slr, TYPE_INTEGER);
BINARYEXPRESSIONHELPER(Or, helper_DeciferType(left, right));
BINARYEXPRESSIONHELPER(Xor, helper_DeciferType(left, right));
BINARYEXPRESSIONHELPER(And, helper_DeciferType(left, right));

class NotExpression : public Expression{
public:
    NotExpression(Expression* value){
        this->value = value;
    }
    void secondpass(){ this->type = value->getType(); };
    void genCode(struct context& context);
private:
    Expression* value;
};

class NegExpression : public Expression{
public:
    NegExpression(Expression* value){
        this->value = value;
    }
    void secondpass();
    void genCode(struct context& context);
private:
    Expression* value;
};

class NNExpression : public Expression{
public:
    NNExpression(Expression* value){
        this->value = value;
    }
    void secondpass();
    void genCode(struct context& context){ nnexpression->genCode(context); }
private:
    Expression* value;
    Expression* nnexpression;
};

class IntegerExpression : public Expression{
public:
    IntegerExpression(int integer){
        this->integer = integer;
    }
    void secondpass(){ this->type = TYPE_INTEGER; }
    void genCode(struct context& context);
private:
    int integer;
};

class BooleanExpression : public Expression{
public:
    BooleanExpression(int boolean){
        this->boolean = boolean;
    }
    void secondpass(){ this->type = TYPE_BOOLEAN; }
    void genCode(struct context& context);
private:
    int boolean;
};

class IdentifierExpression : public Expression{
public:
    IdentifierExpression(string name, Expression* position){
        this->name = name;
        this->position = position;
    }
    IdentifierExpression(string name){
        this->name = name;
        this->position = NULL;
    }
    void secondpass();
    void genCode(struct context& context);
    string getName(){ return name; }
    int getSize(){ return this->array?helper_getSize(name, this->type):1; }
private:
    string name;
    Expression* position;
    bool array;
};

class FunctionExpression : public Expression{
public:
    void addName(string name){ this->name = name; }
    void addParameter(Expression* parameter);
    void genCode(struct context& context);
    void secondpass(){ for(Expression* parameter : parameters) parameter->secondpass(); }
private:
    string name;
    int parameter_count;
    vector<Expression*> parameters;
};

////////////////
// Statements //
////////////////

class Statement : public AST{
public:
    virtual string genCode() = 0;
};

class StatementBlock : public Statement{
public:
    string genCode(){ stringstream code; for(Statement* statement : statements) code << statement->genCode() << endl; return code.str(); };
    void addStatement(Statement* statement){ statements.push_back(statement); }
    void secondpass(){
        for(Statement* statement : statements)
            statement->secondpass();
    }
private:
    vector<Statement*> statements;
};

class ExpressionStatement : public Statement{
public:
    ExpressionStatement(Expression* expression){
        this->expression = expression;
    }
    string genCode();
    void secondpass(){ expression->secondpass(); }
private:
    Expression* expression;
};

class PrintStatement : public Statement{
public:
    void secondpass();
    void printline(bool isprintline){ this->isprintline = isprintline; }
    void addParameter(struct parameter_type& parameter){ this->parameters.push_back(parameter); }
    string genCode();
private:
    int print_id;
    bool isprintline;
    vector<struct parameter_type> parameters;
    void genConstantData();
};

class FunctionStatement : public Statement{
public:
    FunctionStatement(string name, int type, vector<struct function_parameter>* function_params, Statement* body){
        this->name = name;
        this->type = type;
        this->function_params = function_params;
        this->body = body;
    }
    void secondpass();
    string genCode();
private:
    string name;
    int type;
    vector<struct function_parameter>* function_params;
    Statement* body;
};

class IfStatement : public Statement{
public:
    IfStatement(Expression* condition, Statement* trueBlock, Statement* falseBlock){
        this->condition = condition;
        this->trueBlock = trueBlock;
        this->falseBlock = falseBlock;
    }
    void secondpass(){ condition->secondpass(); trueBlock->secondpass(); falseBlock->secondpass(); }
    string genCode();
private:
    Expression* condition;
    Statement* trueBlock;
    Statement* falseBlock;
};

class DeclareStatement : public Statement{
public:
    DeclareStatement(string name, int type, int size){
        this->name = name;
        this->type = type;
        this->size = size;
    }
    void secondpass();
    string genCode();
private:
    string name;
    int type;
    int size;
};

class SetStatement : public Statement{
public:
    SetStatement(string name, Expression* expression, Expression* position){
        this->name = name;
        this->expression = expression;
        this->position = position;
    }
    void secondpass();
    string genCode();
private:
    string name;
    Expression* expression;
    Expression* position;
};

class AssignStatement : public Statement{
public:
    AssignStatement(int variant, string name, Expression* position, int type, Expression* rightside){
        this->variant = variant;
        this->name = name;
        this->position = position;
        this->type = type;
        this->rightside = rightside;
    }
    void secondpass();
    string genCode(){ return statement->genCode(); }
private:
    int variant;
    string name;
    Expression* position;
    int type;
    Expression* rightside;
    Statement* statement;
};

struct context{
    string code;
    string comment;
    bool is_printable; //True if comment should be shown
};

struct parameter_type{
    int type;
    union{
        string* literal;
        Expression* expression;
    };
};

#endif //AST_H