%code requires{
    #include "ast.h"
}

%{
    #include <iostream>
    #include "ast.h"

    using namespace std;

    extern bool helper_isArray(string name);

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
    vector<Expression*>* array_values;
    vector<struct function_parameter>* function_statement_params;
    string* literal;
    int integer;
    bool boolean;
}

%token PARENTHESIS_LEFT PARENTHESIS_RIGHT BRACKET_LEFT BRACKET_RIGHT CURLY_LEFT CURLY_RIGHT COMA SEMICOLON DOUBLE_COLON NEWLINE
%token OPERATOR_ADD OPERATOR_SUB OPERATOR_MUL OPERATOR_DIV OPERATOR_MOD OPERATOR_POW
%token OPERATOR_SAL OPERATOR_SLR OPERATOR_SAR OPERATOR_OR OPERATOR_XOR OPERATOR_AND OPERATOR_NOT
%token COMPARISON_GT COMPARISON_LT COMPARISON_EQ COMPARISON_GE COMPARISON_LE COMPARISON_NE COMPARISON_AND COMPARISON_OR OPERATOR_NEG
%token KW_PRINT KW_PRINTLN KW_ARRAY KW_IF KW_ELSEIF KW_ELSE KW_FUNCTION KW_END
%token LITERAL IDENTIFIER INTEGER BOOLEAN
%token TYPE OPERATOR_ASSIGN

%type<statement> optional_statements statement_list statement print print_params assign else
%type<expression> condition condition_ooo_l1 expression expression_ooo_l1 expression_ooo_l2 expression_ooo_l3 expression_ooo_l4 expression_ooo_l5 expression_ooo_l6 final_value optional_function_expression_params function_expression_params
%type<array_values> array
%type<function_statement_params> optional_function_statement_params function_statement_params
%type<literal> LITERAL IDENTIFIER
%type<integer> INTEGER TYPE
%type<boolean> BOOLEAN

%%
initial: optional_statements { code_tree = $1; }
    ;

optional_statements: optional_statement_separators statement_list optional_statement_separators { $$ = $2; }
    | optional_statement_separators { $$ = new StatementBlock(); } //Set $$ = NULL to create no code.
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

statement: print { $$ = $1; }
    | condition { $$ = new ExpressionStatement($1); }
    | assign { $$ = $1; }
    | KW_IF condition optional_statements else KW_END { $$ = new IfStatement($2, $3, $4); }
    | KW_FUNCTION IDENTIFIER PARENTHESIS_LEFT optional_function_statement_params PARENTHESIS_RIGHT DOUBLE_COLON TYPE optional_statements KW_END { $$ = new FunctionStatement(*$2, $7, $4, $8); }
    ;
    
print: KW_PRINT PARENTHESIS_LEFT print_params PARENTHESIS_RIGHT { $$ = $3; ((PrintStatement*)$$)->printline(false); }
    | KW_PRINTLN PARENTHESIS_LEFT print_params PARENTHESIS_RIGHT { $$ = $3; ((PrintStatement*)$$)->printline(true); }
    ;

print_params: print_params COMA optional_newlines LITERAL { $$ = $1; struct parameter_type parameter; parameter.type = TYPE_LITERAL; parameter.literal = new string(*$4); ((PrintStatement*)$$)->addParameter(parameter);  }
    | print_params COMA optional_newlines condition { $$ = $1; struct parameter_type parameter; parameter.type = $4->getType(); parameter.expression = $4; ((PrintStatement*)$$)->addParameter(parameter);  }
    | LITERAL { $$ = new PrintStatement(); struct parameter_type parameter; parameter.type = TYPE_LITERAL; parameter.literal = new string(*$1); ((PrintStatement*)$$)->addParameter(parameter); }
    | condition { $$ = new PrintStatement(); struct parameter_type parameter; parameter.type = $1->getType(); parameter.expression = $1; ((PrintStatement*)$$)->addParameter(parameter); }
    ;

assign: IDENTIFIER DOUBLE_COLON TYPE { $$ = new DeclareStatement(*$1, $3, 1); }
    | IDENTIFIER DOUBLE_COLON TYPE OPERATOR_ASSIGN optional_newlines condition {
        if($6->getSize() > 1){
            std::cerr << "ERR: Incompatible sizes on `" << *$1 << "` assignation!" << std::endl;
            exit(1);
        }
        $$ = new StatementBlock();
        ((StatementBlock*)$$)->addStatement(new DeclareStatement(*$1, $3, 1));
        ((StatementBlock*)$$)->addStatement(new SetStatement(*$1, $6, new IntegerExpression(1)));
    }
    | IDENTIFIER BRACKET_LEFT condition BRACKET_RIGHT OPERATOR_ASSIGN optional_newlines condition {
        if($7->getSize() > 1){
            std::cerr << "ERR: Incompatible sizes on `" << *$1 << "` assignation!" << std::endl;
            exit(1);
        }
        $$ = new SetStatement(*$1, $7, $3);
    }
    | IDENTIFIER DOUBLE_COLON KW_ARRAY CURLY_LEFT TYPE CURLY_RIGHT PARENTHESIS_LEFT INTEGER PARENTHESIS_RIGHT { $$ = new DeclareStatement(*$1, $5, $8); }
    | IDENTIFIER OPERATOR_ASSIGN optional_newlines condition {
        int size = $4->getSize();
        if(size != helper_getSize(*$1, TYPE_BOOLEAN)){
            std::cerr << "ERR: Incompatible sizes on `" << *$1 << "` assignation!" << std::endl;
            exit(1);
        }
        if(size == 1)
            $$ = new SetStatement(*$1, $4, new IntegerExpression(1));
        else{
            $$ = new StatementBlock();
            for(int position = 1; position <= size; position++)
                ((StatementBlock*)$$)->addStatement(new SetStatement(*$1, new IdentifierExpression(((IdentifierExpression*)$4)->getName(), new IntegerExpression(position)), new IntegerExpression(position)));
        }
    }
    | IDENTIFIER DOUBLE_COLON KW_ARRAY CURLY_LEFT TYPE CURLY_RIGHT OPERATOR_ASSIGN optional_newlines IDENTIFIER {
        int size = helper_getSize(*$9, $5);
        $$ = new StatementBlock();
        ((StatementBlock*)$$)->addStatement(new DeclareStatement(*$1, $5, size));
        for(int position = 1; position <= size; position++)
            ((StatementBlock*)$$)->addStatement(new SetStatement(*$1, new IdentifierExpression(*$9, new IntegerExpression(position)), new IntegerExpression(position))); 
    }
    | IDENTIFIER DOUBLE_COLON KW_ARRAY CURLY_LEFT TYPE CURLY_RIGHT OPERATOR_ASSIGN optional_newlines BRACKET_LEFT array BRACKET_RIGHT   { 
        $$ = new StatementBlock();
        ((StatementBlock*)$$)->addStatement(new DeclareStatement(*$1, $5, $10->size()));
        int position = 1;
        for(Expression* expression : *$10)
            ((StatementBlock*)$$)->addStatement(new SetStatement(*$1, expression, new IntegerExpression(position++)));
    }
    ;

array: array COMA optional_newlines condition { $$ = $1; $$->push_back($4); }
    | condition { $$ = new vector<Expression*>; $$->push_back($1); }
    ;

else: KW_ELSEIF condition optional_statements else { $$ = new IfStatement($2, $3, $4); }
    | KW_ELSE optional_statements { $$ = $2; }
    | { $$ = new StatementBlock(); }
    ;

optional_function_statement_params: function_statement_params { $$ = $1; }
    | { $$ = new vector<function_parameter>; }
    ;

function_statement_params: function_statement_params COMA optional_newlines IDENTIFIER DOUBLE_COLON TYPE { $$ = $1; struct function_parameter fp; fp.name = *$4; fp.type = $6; $$->push_back(fp); }
    | IDENTIFIER DOUBLE_COLON TYPE { $$ = new vector<function_parameter>; struct function_parameter fp; fp.name = *$1; fp.type = $3; $$->push_back(fp); }
    ;

condition: condition_ooo_l1 { $$ = $1; }
    | condition COMPARISON_AND optional_newlines condition_ooo_l1 { if($1->getType() != TYPE_BOOLEAN || $4->getType() != TYPE_BOOLEAN) yyerror("non-boolean used in boolean context"); $$ = new ComparisonAndExpression($1, $4); }
    | condition COMPARISON_OR optional_newlines condition_ooo_l1 { if($1->getType() != TYPE_BOOLEAN || $4->getType() != TYPE_BOOLEAN) yyerror("non-boolean used in boolean context"); $$ = new ComparisonOrExpression($1, $4); }
    ;

condition_ooo_l1: expression { $$ = $1; }
    | condition_ooo_l1 COMPARISON_GT optional_newlines expression { $$ = new GTExpression($1, $4); }
    | condition_ooo_l1 COMPARISON_LT optional_newlines expression { $$ = new LTExpression($1, $4); }
    | condition_ooo_l1 COMPARISON_EQ optional_newlines expression { $$ = new EQExpression($1, $4); }
    | condition_ooo_l1 COMPARISON_GE optional_newlines expression { $$ = new GEExpression($1, $4); }
    | condition_ooo_l1 COMPARISON_LE optional_newlines expression { $$ = new LEExpression($1, $4); }
    | condition_ooo_l1 COMPARISON_NE optional_newlines expression { $$ = new NEExpression($1, $4); }
    ;

expression: expression_ooo_l1 { $$ = $1; }
    | expression OPERATOR_ADD optional_newlines expression_ooo_l1 { $$ = new AddExpression($1, $4); }
    | expression OPERATOR_SUB optional_newlines expression_ooo_l1 { $$ = new SubExpression($1, $4); }
    ;

expression_ooo_l1: expression_ooo_l2 { $$ = $1; }
    | expression_ooo_l1 OPERATOR_OR optional_newlines expression_ooo_l2 { $$ = new OrExpression($1, $4); }
    | expression_ooo_l1 OPERATOR_XOR optional_newlines expression_ooo_l2 { $$ = new XorExpression($1, $4); }
    ;

expression_ooo_l2: expression_ooo_l3 { $$ = $1; }
    | expression_ooo_l2 OPERATOR_SAL optional_newlines expression_ooo_l3 { $$ = new SalExpression($1, $4); }
    | expression_ooo_l2 OPERATOR_SAR optional_newlines expression_ooo_l3 { $$ = new SarExpression($1, $4); }
    | expression_ooo_l2 OPERATOR_SLR optional_newlines expression_ooo_l3 { $$ = new SlrExpression($1, $4); }
    ;

expression_ooo_l3: expression_ooo_l4 { $$ = $1; }
    | expression_ooo_l3 OPERATOR_AND optional_newlines expression_ooo_l4 { $$ = new AndExpression($1, $4); }
    ;

expression_ooo_l4: expression_ooo_l5 { $$ = $1; }
    | expression_ooo_l4 OPERATOR_MUL optional_newlines expression_ooo_l5 { $$ = new MulExpression($1, $4); }
    | expression_ooo_l4 OPERATOR_DIV optional_newlines expression_ooo_l5 { $$ = new DivExpression($1, $4); }
    | expression_ooo_l4 OPERATOR_MOD optional_newlines expression_ooo_l5 { $$ = new ModExpression($1, $4); }
    ;

expression_ooo_l5: expression_ooo_l6 { $$ = $1; }
    | expression_ooo_l6 OPERATOR_POW optional_newlines expression_ooo_l5 { $$ = new PowExpression($1, $4); }
    ;

expression_ooo_l6: final_value { $$ = $1; }
    | OPERATOR_NOT expression_ooo_l6 { if($2->getType() != TYPE_BOOLEAN) $$ = new NotExpression($2); else $$ = new NegExpression($2); }
    | OPERATOR_NEG expression_ooo_l6 { if($2->getType() != TYPE_BOOLEAN) yyerror("Cannot negate non-boolean value!"); $$ = new NegExpression($2); }
    | OPERATOR_ADD expression_ooo_l6 { $$ = $2; }
    | OPERATOR_SUB expression_ooo_l6 { $$ = new MulExpression(new IntegerExpression(-1), $2); }
    ;

final_value: PARENTHESIS_LEFT condition PARENTHESIS_RIGHT { $$ = $2; }
    | BOOLEAN { $$ = new BooleanExpression($1); }
    | INTEGER { $$ = new IntegerExpression($1); }
    | IDENTIFIER { $$ = new IdentifierExpression(*$1); }
    | IDENTIFIER BRACKET_LEFT condition BRACKET_RIGHT { $$ = new IdentifierExpression(*$1, $3); }
    | IDENTIFIER PARENTHESIS_LEFT optional_function_expression_params PARENTHESIS_RIGHT { $$ = $3; ((FunctionExpression*)$$)->addName(*$1); }
    ;

optional_function_expression_params: function_expression_params { $$ = $1; }
    | { $$ = new FunctionExpression(); }
    ;

function_expression_params: function_expression_params COMA optional_newlines final_value { $$ = $1; ((FunctionExpression*)$$)->addParameter($4); }
    | final_value { $$ = new FunctionExpression(); ((FunctionExpression*)$$)->addParameter($1); }
    ;

%%