#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

using namespace std;

extern vector<string> constant_data;

struct context{
    string code;
    string comment;
    bool printable; //True if comment should be shown
};

class AST{

};


/////////////////
// Expressions //
/////////////////

class Expression : public AST{
public:
    virtual void genCode(struct context& context) = 0;
};

// class LiteralExpression : public Expression{
// public:
//     LiteralExpression(string literal){
//         this->literal = literal;
//     }
//     void genCode(struct context& context);
// private:
//     string literal;
// };


////////////////
// Statements //
////////////////

class Statement : public AST{
public:
    virtual string genCode() = 0;
};

class PrintStatement : public Statement{
public:
    PrintStatement(string* params){
        this->print_id = constant_data.size();
        this->params = *params;
        genConstantData();
    }
    string genCode();
private:
    int print_id;
    string params;
    void genConstantData();
};

#endif //AST_H