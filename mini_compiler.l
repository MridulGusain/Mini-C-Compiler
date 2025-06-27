%{
#include "mini_compiler.tab.h"
#include <string.h>
#include <stdlib.h>

int lineno = 1;
%}

%%

"int"       { return INT; }
"char"      { return CHAR; }
"float"     { return FLOAT; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"for"       { return FOR; }
"return"    { return RETURN; }
"print"     { return PRINT; }

"=="        { return EQ; }
"!="        { return NEQ; }
"<="        { return LE; }
">="        { return GE; }
"="         { return ASSIGN; }
"<"         { return '<'; }
">"         { return '>'; }
"+"         { return '+'; }
"-"         { return '-'; }
"\/"        { return '/'; }        
";"         { return ';'; }
","         { return ','; }
"("         { return '('; }
")"         { return ')'; }
"{"         { return '{'; }
"}"         { return '}'; }

[0-9]+      {
                yylval.ival = atoi(yytext);
                return NUMBER;
            }

[a-zA-Z_][a-zA-Z0-9_]* {
                yylval.sval = strdup(yytext);
                return ID;
            }

[ \t\r]+    { /* Skip whitespace */ }
\n          { lineno++; }

.           { printf("Unknown character: %s\n", yytext); }

%%

int yywrap() {
    return 1;
}

