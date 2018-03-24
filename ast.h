#ifndef AST_H
#define AST_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

class AST{};


/////////////////
// Expressions //
/////////////////

class Expression : public AST{
public:
    virtual void genCode(struct context& context) = 0;
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

class AddExpression : public BinaryExpression{
public:
    AddExpression(Expression* left, Expression* right) : BinaryExpression(left, right){}
    void genCode(struct context& context);
};

class SubExpression : public BinaryExpression{
public:
    SubExpression(Expression* left, Expression* right) : BinaryExpression(left, right){}
    void genCode(struct context& context);
};

class IntegerExpression : public Expression{
public:
    IntegerExpression(int integer){
        this->integer = integer;
    }
    void genCode(struct context& context);
private:
    int integer;
};

class BooleanExpression : public Expression{
public:
    BooleanExpression(bool boolean){
        this->boolean = boolean;
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
};

class StatementBlock : public Statement{
public:
    string genCode(){ stringstream code; for(Statement* statement : statements) code << statement->genCode() << endl; return code.str(); };
    void addStatement(Statement* statement){ statements.push_back(statement); }
private:
    vector<Statement*> statements;
};

class PrintStatement : public Statement{
public:
    PrintStatement(vector<struct parameter_type>* parameters, bool isprintline){
        this->isprintline = isprintline;
        this->parameters = *parameters;
        genConstantData();
    }
    string genCode();
private:
    int print_id;
    bool isprintline;
    vector<struct parameter_type> parameters;
    void genConstantData();
};


enum{
    TYPE_LITERAL,
    TYPE_INTEGER,
    TYPE_BOOLEAN
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