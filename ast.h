#ifndef AST_H
#define AST_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

enum{
    TYPE_LITERAL,
    TYPE_NUMBER
};

struct context{
    string code;
    string comment;
    bool printable; //True if comment should be shown
};

struct parameter_type{
    int type;
    union{
        string* literal;
        int number;
    };
};

class AST{};


/////////////////
// Expressions //
/////////////////

class Expression : public AST{
public:
    virtual void genCode(struct context& context) = 0;
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

#endif //AST_H