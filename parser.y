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
    vector<struct parameter_type>* print_params;
    string* literal;
    int number;
}

%token PARENTHESIS_LEFT PARENTHESIS_RIGHT COMA SEMICOLON NEWLINE
%token KW_PRINT KW_PRINTLN
%token LITERAL NUMBER

%type<statement>statement_list statement print
%type<print_params> print_params
%type<literal> LITERAL
%type<number> NUMBER

%%
initial: statement_list { code_tree = $1; }
    ;

statement_separator: statement_separator NEWLINE
    | statement_separator SEMICOLON
    | NEWLINE
    | SEMICOLON
    ;

optional_newlines: many_newlines
    |
    ;

many_newlines: many_newlines NEWLINE
    | NEWLINE
    ;

statement_list: statement_list statement_separator statement { $$ = $1; ((StatementBlock*)$$)->addStatement($3); }
    | statement { $$ = new StatementBlock(); ((StatementBlock*)$$)->addStatement($1); }
    ;

statement: print { $$ = $1; };

print: KW_PRINT PARENTHESIS_LEFT print_params PARENTHESIS_RIGHT { $$ = new PrintStatement($3, false); }
    | KW_PRINTLN PARENTHESIS_LEFT print_params PARENTHESIS_RIGHT { $$ = new PrintStatement($3, true); }
    ;

print_params: print_params COMA optional_newlines LITERAL { $$ = $1; struct parameter_type parameter; parameter.type = TYPE_LITERAL; parameter.literal = new string(*$4); $$->push_back(parameter);  }
    | print_params COMA optional_newlines NUMBER { $$ = $1; struct parameter_type parameter; parameter.type = TYPE_NUMBER; parameter.number = $4; $$->push_back(parameter);  }
    | LITERAL { $$ = new vector<struct parameter_type>; struct parameter_type parameter; parameter.type = TYPE_LITERAL; parameter.literal = new string(*$1); $$->push_back(parameter); }
    | NUMBER { $$ = new vector<struct parameter_type>; struct parameter_type parameter; parameter.type = TYPE_NUMBER; parameter.number = $1; $$->push_back(parameter); }
    ;
%%