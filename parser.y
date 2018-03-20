%code requires{
    #include "ast.h"
}

%{
    #include <iostream>
    #include "ast.h"

    using namespace std;

    Statement* code_tree;
    extern int yylineno;
    int yylex();
    int yyerror(const char* msg){
        cerr << yylineno << ": " << msg << endl;
        exit(1);
    }
%}

%union{
    string* literal;
}

%token PARENTHESIS_LEFT PARENTHESIS_RIGHT
%token KW_PRINT KW_PRINTLN
%token LITERAL

%type<literal> params LITERAL

%%
print: KW_PRINT PARENTHESIS_LEFT params PARENTHESIS_RIGHT { code_tree = new PrintStatement($3, false); }
    | KW_PRINTLN PARENTHESIS_LEFT params PARENTHESIS_RIGHT { code_tree = new PrintStatement($3, true); }
    ;

params: LITERAL { $$ = $1; }
%%