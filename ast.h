#ifndef AST_H
#define AST_H

#define BINARYEXPRESSIONHELPER(name, valuetype)                                                                             \
    class name##Expression : public BinaryExpression{                                                                       \
    public:                                                                                                                 \
        name##Expression(Expression* left, Expression* right) : BinaryExpression(left, right){ this->type = valuetype; }    \
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

class AST{};

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

 
extern void helper_DeclareVariable(string name, int type, int size);
extern void helper_SetVariable(string name, int type, Expression* position);
extern int helper_UseVariable(string name);
extern void helper_DeclareFunction(string name, vector<struct function_parameter>* function_params);
extern int helper_DeciferType(Expression* left, Expression* right);
extern int helper_getSize(string name, int type);

class FunctionExpression : public Expression{
public:
    void addName(string name){ this->name = name; }
    void addParameter(Expression* parameter);
    void genCode(struct context& context);
private:
    string name;
    int parameter_count;
    vector<Expression*> parameters;
};

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
BINARYEXPRESSIONHELPER(ComparisonOr, TYPE_BOOLEAN);
BINARYEXPRESSIONHELPER(ComparisonAnd, TYPE_BOOLEAN);

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
        this->type = value->getType();
    }
    void genCode(struct context& context);
private:
    Expression* value;
};

class NegExpression : public Expression{
public:
    NegExpression(Expression* value){
        this->value = value;
        this->type = TYPE_BOOLEAN;
    }
    void genCode(struct context& context);
private:
    Expression* value;
};

class IntegerExpression : public Expression{
public:
    IntegerExpression(int integer){
        this->integer = integer;
        this->type = TYPE_INTEGER;
    }
    void genCode(struct context& context);
private:
    int integer;
};

class BooleanExpression : public Expression{
public:
    BooleanExpression(int boolean){
        this->boolean = boolean;
        this->type = TYPE_BOOLEAN;
    }
    void genCode(struct context& context);
private:
    int boolean;
};

class IdentifierExpression : public Expression{
public:
    IdentifierExpression(string name, Expression* position){
        this->type = helper_UseVariable(name);
        this->name = name;
        this->position = position;
    }
    IdentifierExpression(string name){
        this->type = helper_UseVariable(name);
        this->name = name;
        this->array = helper_getSize(name, this->type) == 1?false:true;
        this->position = new IntegerExpression(array);
    }
    void genCode(struct context& context);
    string getName(){ return name; }
    int getSize(){ return this->array?helper_getSize(name, this->type):1; }
private:
    string name;
    Expression* position;
    bool array;
};

////////////////
// Statements //
////////////////

class Statement : public AST{
public:
    virtual string genCode() = 0;
    virtual void secondpass(){};
};

class StatementBlock : public Statement{
public:
    string genCode(){ stringstream code; for(Statement* statement : statements) code << statement->genCode() << endl; return code.str(); };
    void addStatement(Statement* statement){ statements.push_back(statement); }
    void secondpass(){ for(Statement* statement : statements) statement->secondpass(); }
private:
    vector<Statement*> statements;
};

class ExpressionStatement : public Statement{
public:
    ExpressionStatement(Expression* expression){
        this->expression = expression;
    }
    string genCode();
private:
    Expression* expression;
};

class PrintStatement : public Statement{
public:
    PrintStatement(){}
    void secondpass(){ this->genConstantData(); }
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
        helper_DeclareFunction(name, function_params);
        this->name = name;
        this->type = type;
        this->body = body;
    }
    void secondpass();
    string genCode(){ return string(""); };
private:
    string name;
    int type;
    Statement* body;
};

class IfStatement : public Statement{
public:
    IfStatement(Expression* condition, Statement* trueBlock, Statement* falseBlock){
        this->condition = condition;
        this->trueBlock = trueBlock;
        this->falseBlock = falseBlock;
    }
    void secondpass(){ trueBlock->secondpass(); falseBlock->secondpass(); }
    string genCode();
private:
    Expression* condition;
    Statement* trueBlock;
    Statement* falseBlock;
};

class DeclareStatement : public Statement{
public:
    DeclareStatement(string name, int type, int size){
        helper_DeclareVariable(name, type, size);
        this->name = name;
        this->size = size;
    }
    string genCode();
private:
    string name; //Used for comment only
    int size;
};

class SetStatement : public Statement{
public:
    SetStatement(string name, Expression* expression, Expression* position){
        helper_SetVariable(name, expression->getType(), position);
        this->name = name;
        this->expression = expression;
        this->position = position;
    }
    string genCode();
private:
    string name;
    Expression* expression;
    Expression* position;
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