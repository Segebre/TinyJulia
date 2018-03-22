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
    Expression* expression;
    Statement* statement;
    vector<struct parameter_type>* print_params;
    string* literal;
    int integer;
    bool boolean;
}

%token PARENTHESIS_LEFT PARENTHESIS_RIGHT COMA SEMICOLON NEWLINE
%token KW_PRINT KW_PRINTLN
%token LITERAL INTEGER BOOLEAN

%type<statement>statement_list statement print
%type<expression>non_bool_integer boolean
%type<print_params> print_params
%type<literal> LITERAL
%type<integer> INTEGER //integer
%type<boolean> BOOLEAN

%%
initial: optional_statement_separators statement_list optional_statement_separators { code_tree = $2; }
    | optional_statement_separators { code_tree = new StatementBlock(); } //Set code_tree = NULL to create no code.
    ;

optional_statement_separators: statement_separator
    |
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
    | print_params COMA optional_newlines non_bool_integer { $$ = $1; struct parameter_type parameter; parameter.type = TYPE_INTEGER; parameter.expression = $4; $$->push_back(parameter);  }
    | print_params COMA optional_newlines boolean { $$ = $1; struct parameter_type parameter; parameter.type = TYPE_BOOLEAN; parameter.expression = $4; $$->push_back(parameter);  }
    | LITERAL { $$ = new vector<struct parameter_type>; struct parameter_type parameter; parameter.type = TYPE_LITERAL; parameter.literal = new string(*$1); $$->push_back(parameter); }
    | non_bool_integer { $$ = new vector<struct parameter_type>; struct parameter_type parameter; parameter.type = TYPE_INTEGER; parameter.expression = $1; $$->push_back(parameter); }
    | boolean { $$ = new vector<struct parameter_type>; struct parameter_type parameter; parameter.type = TYPE_BOOLEAN; parameter.expression = $1; $$->push_back(parameter); }
    ;

// integer: non_bool_integer { $$ = $1; }
//     | boolean { $$ = $1; }
//     ;

non_bool_integer: INTEGER { $$ = new IntegerExpression($1); }
    ;

boolean: BOOLEAN { $$ = new BooleanExpression($1); }
    ;
%%