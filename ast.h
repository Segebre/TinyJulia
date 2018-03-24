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

class AST{};


/////////////////
// Expressions //
/////////////////

class Expression : public AST{
public:
    virtual void genCode(struct context& context) = 0;
    int getType(){ return this->type; }
protected:
    int type;
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

////////////////
// Statements //
////////////////

class Statement : public AST{
public:
    virtual string genCode() = 0;
    virtual void secondpass() = 0;
};

class StatementBlock : public Statement{
public:
    string genCode(){ stringstream code; for(Statement* statement : statements) code << statement->genCode() << endl; return code.str(); };
    void addStatement(Statement* statement){ statements.push_back(statement); }
    void secondpass(){ for(Statement* statement : statements) statement->secondpass(); }
private:
    vector<Statement*> statements;
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