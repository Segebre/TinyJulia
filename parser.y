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
    Statement* statement;
    string* literal;
}

%token PARENTHESIS_LEFT PARENTHESIS_RIGHT SEMICOLON NEWLINE
%token KW_PRINT KW_PRINTLN
%token LITERAL

%type<statement>statement_list statement print
%type<literal> params LITERAL

%%
initial: statement_list { code_tree = $1; }
    ;

statement_separator: statement_separator NEWLINE
    | statement_separator SEMICOLON
    | NEWLINE
    | SEMICOLON
    ;

statement_list: statement_list statement_separator statement { $$ = $1; ((StatementBlock*)$$)->addStatement($3); }
    | statement { $$ = new StatementBlock(); ((StatementBlock*)$$)->addStatement($1); }
    ;

statement: print { $$ = $1; }

print: KW_PRINT PARENTHESIS_LEFT params PARENTHESIS_RIGHT { $$ = new PrintStatement($3, false); }
    | KW_PRINTLN PARENTHESIS_LEFT params PARENTHESIS_RIGHT { $$ = new PrintStatement($3, true); }
    ;

params: LITERAL { $$ = $1; }
%%