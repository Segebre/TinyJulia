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
    string* literal;
    int integer;
    bool boolean;
}

%token PARENTHESIS_LEFT PARENTHESIS_RIGHT COMA SEMICOLON NEWLINE
%token OPERATOR_ADD OPERATOR_SUB OPERATOR_MUL OPERATOR_DIV OPERATOR_MOD OPERATOR_POW
%token COMPARISON_GT COMPARISON_LT  COMPARISON_EQ  COMPARISON_GE COMPARISON_LE COMPARISON_NE
%token KW_PRINT KW_PRINTLN
%token LITERAL INTEGER BOOLEAN

%type<statement> statement_list statement print print_params
%type<expression> condition expression expression_ooo_l1 expression_ooo_l2 expression_ooo_l3 expression_ooo_l4 expression_ooo_l5 expression_ooo_l6 final_value
%type<literal> LITERAL
%type<integer> INTEGER
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

print: KW_PRINT PARENTHESIS_LEFT print_params PARENTHESIS_RIGHT { $$ = $3; ((PrintStatement*)$$)->printline(false); }
    | KW_PRINTLN PARENTHESIS_LEFT print_params PARENTHESIS_RIGHT { $$ = $3; ((PrintStatement*)$$)->printline(true); }
    ;

print_params: print_params COMA optional_newlines LITERAL { $$ = $1; struct parameter_type parameter; parameter.type = TYPE_LITERAL; parameter.literal = new string(*$4); ((PrintStatement*)$$)->addParameter(parameter);  }
    | print_params COMA optional_newlines condition { $$ = $1; struct parameter_type parameter; parameter.type = $4->getType(); parameter.expression = $4; ((PrintStatement*)$$)->addParameter(parameter);  }
    | LITERAL { $$ = new PrintStatement(); struct parameter_type parameter; parameter.type = TYPE_LITERAL; parameter.literal = new string(*$1); ((PrintStatement*)$$)->addParameter(parameter); }
    | condition { $$ = new PrintStatement(); struct parameter_type parameter; parameter.type = $1->getType(); parameter.expression = $1; ((PrintStatement*)$$)->addParameter(parameter); }
    ;

condition: expression { $$ = $1; }
    | condition COMPARISON_GT expression { $$ = new GTExpression($1, $3); }
    | condition COMPARISON_LT expression { $$ = new LTExpression($1, $3); }
    | condition COMPARISON_EQ expression { $$ = new EQExpression($1, $3); }
    | condition COMPARISON_GE expression { $$ = new GEExpression($1, $3); }
    | condition COMPARISON_LE expression { $$ = new LEExpression($1, $3); }
    | condition COMPARISON_NE expression { $$ = new NEExpression($1, $3); }
    ;

expression: expression_ooo_l1 { $$ = $1; }
    | expression OPERATOR_ADD expression_ooo_l1 { $$ = new AddExpression($1, $3); }
    | expression OPERATOR_SUB expression_ooo_l1 { $$ = new SubExpression($1, $3); }
    ;

expression_ooo_l1: expression_ooo_l2 { $$ = $1; }
    ;

expression_ooo_l2: expression_ooo_l3 { $$ = $1; }
    ;

expression_ooo_l3: expression_ooo_l4 { $$ = $1; }
    ;

expression_ooo_l4: expression_ooo_l5 { $$ = $1; }
    | expression_ooo_l4 OPERATOR_MUL expression_ooo_l5 { $$ = new MulExpression($1, $3); }
    | expression_ooo_l4 OPERATOR_DIV expression_ooo_l5 { $$ = new DivExpression($1, $3); }
    | expression_ooo_l4 OPERATOR_MOD expression_ooo_l5 { $$ = new ModExpression($1, $3); }
    ;

expression_ooo_l5: expression_ooo_l6 { $$ = $1; }
    | expression_ooo_l6 OPERATOR_POW expression_ooo_l5 { $$ = new PowExpression($1, $3); }
    ;

expression_ooo_l6: final_value { $$ = $1; }
    ;

final_value: PARENTHESIS_LEFT condition PARENTHESIS_RIGHT { $$ = $2; }
    | BOOLEAN { $$ = new BooleanExpression($1); }
    | OPERATOR_ADD BOOLEAN { $$ = new IntegerExpression($2); }
    | OPERATOR_SUB BOOLEAN { $$ = new IntegerExpression($2*-1); }
    | INTEGER { $$ = new IntegerExpression($1); }
    | OPERATOR_ADD INTEGER { $$ = new IntegerExpression($2); }
    | OPERATOR_SUB INTEGER { $$ = new IntegerExpression($2*-1); }
    ;
%%