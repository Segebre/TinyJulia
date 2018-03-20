%option noyywrap
%option yylineno

%{
    #include <string>
    #include <iostream>
    #include "tokens.h"
    string literal;
%}

DIGIT [[:digit:]]

%x literal_parsing

%%
"("             { return PARENTHESIS_LEFT; }
")"             { return PARENTHESIS_RIGHT; }
","             { return COMA; }
";"             { return SEMICOLON; }
"\n"            { return NEWLINE; }
"true"          { yylval.boolean = true; return BOOLEAN; }
"false"         { yylval.boolean = false; return BOOLEAN; }
"print"         { return KW_PRINT; }
"println"       { return KW_PRINTLN; }
"-"?{DIGIT}+    { yylval.number = atoi(yytext); return NUMBER; }
\"              { literal = "\""; BEGIN(literal_parsing); }

<literal_parsing>\"         { literal += "\""; BEGIN(INITIAL); yylval.literal = &literal; return LITERAL; }
<literal_parsing>\\\\       { literal += "\", 92, \""; }
<literal_parsing>\\\"       { literal += "\", 34, \""; }
<literal_parsing>\\"t"      { literal += "\", 9, \""; }
<literal_parsing>\\"n"      { literal += "\", 10, \""; }
<literal_parsing>"\\".      { std::cerr << "Unsopported scape sequence `" << yytext << "`" << std::endl; exit(1); }
<literal_parsing>[^\\\"]+   { literal += yytext; }

%%