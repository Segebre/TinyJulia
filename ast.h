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

class PrintStatement : public Statement{
public:
    PrintStatement(string* params, bool isprintline){
        this->print_id = constant_data.size();
        this->isprintline = isprintline;
        this->params = *params;
        genConstantData();
    }
    string genCode();
private:
    int print_id;
    bool isprintline;
    string params;
    void genConstantData();
};

#endif //AST_H