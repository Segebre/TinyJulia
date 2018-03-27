%option noyywrap
%option yylineno

%{
    #include <string>
    #include <iostream>
    #include "tokens.h"
    string literal;
%}

DIGIT [[:digit:]]
ID [[:alpha:]_][[:alnum:]_]*

%x multiline_comment
%x literal_parsing

%%
[ \t]               {  }
"#"$                {  }
"#"[^=\n][^\n]*     {  }
"#="                { BEGIN(multiline_comment); }
"=#"                { std::cerr << yylineno << ": Unpaired end of block comment" << std::endl; exit(1);  }
"("                 { return PARENTHESIS_LEFT; }
")"                 { return PARENTHESIS_RIGHT; }
"="                 { return OPERATOR_ASSIGN; }
"+"                 { return OPERATOR_ADD; }
"-"                 { return OPERATOR_SUB; }
"*"                 { return OPERATOR_MUL; }
"/"                 { return OPERATOR_DIV; }
"%"                 { return OPERATOR_MOD; }
"^"                 { return OPERATOR_POW; }
"<<"                { return OPERATOR_SAL; }
">>"                { return OPERATOR_SAR; }
"|"                 { return OPERATOR_OR; }
"&"                 { return OPERATOR_AND; }
"~"                 { return OPERATOR_NOT; }
">"                 { return COMPARISON_GT; }
"<"                 { return COMPARISON_LT; }
"=="                { return COMPARISON_EQ; }
">="                { return COMPARISON_GE; }
"<="                { return COMPARISON_LE; }
"!="                { return COMPARISON_NE; }
"!"                 { return OPERATOR_NEG; }
","                 { return COMA; }
";"                 { return SEMICOLON; }
"\n"                { return NEWLINE; }
"true"              { yylval.boolean = true; return BOOLEAN; }
"false"             { yylval.boolean = false; return BOOLEAN; }
"print"             { return KW_PRINT; }
"println"           { return KW_PRINTLN; }
"::"                { return DOUBLE_COLON; }
"Int"               { yylval.integer = TYPE_INTEGER; return TYPE; }
"Bool"              { yylval.integer = TYPE_BOOLEAN; return TYPE; }
{ID}                { yylval.literal = new string(yytext); return IDENTIFIER; }
{DIGIT}+            { yylval.integer = atoi(yytext); return INTEGER; }
\"                  { literal = "\""; BEGIN(literal_parsing); }
.                   { std::cerr << yylineno << ": Unexpected `" << yytext << "`" << std::endl; exit(1); }

<multiline_comment>[^=]     {  }
<multiline_comment>"="      {  }
<multiline_comment>"=#"     { BEGIN(INITIAL); }
<multiline_comment><<EOF>>  { std::cerr << yylineno << ": Unended block comment" << std::endl; exit(1);  }

<literal_parsing>\"         { literal += "\""; BEGIN(INITIAL); yylval.literal = &literal; return LITERAL; }
<literal_parsing>\\\\       { literal += "\", 92, \""; }
<literal_parsing>\\\"       { literal += "\", 34, \""; }
<literal_parsing>\\"t"      { literal += "\", 9, \""; }
<literal_parsing>\\"n"      { literal += "\", 10, \""; }
<literal_parsing>"\\".      { std::cerr << yylineno << ": Unsopported scape sequence `" << yytext << "`" << std::endl; exit(1); }
<literal_parsing>[^\\\"]+   { literal += yytext; }

%%