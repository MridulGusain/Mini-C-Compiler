%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[32];
    int value;
} Variable;

Variable vars[100];
int var_count = 0;

int get_var_index(const char* name) {
    for (int i = 0; i < var_count; ++i)
        if (strcmp(vars[i].name, name) == 0)
            return i;
    return -1;
}

void set_var(const char* name, int val) {
    int idx = get_var_index(name);
    if (idx == -1) {
        strcpy(vars[var_count].name, name);
        vars[var_count++].value = val;
    } else {
        vars[idx].value = val;
    }
}

int get_var(const char* name) {
    int idx = get_var_index(name);
    if (idx != -1)
        return vars[idx].value;
    return 0;
}

void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
}

int yylex(void);
%}

%union {
    int ival;
    char* sval;
}

%token <sval> ID
%token <ival> NUMBER
%token INT CHAR FLOAT PRINT IF ELSE WHILE FOR RETURN
%token EQ NEQ LE GE ASSIGN

%type <ival> expr bool_expr

%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%%

program:
      program stmt
    | /* empty */
    ;

stmt:
      INT ID ASSIGN expr ';' {
          set_var($2, $4);
          printf("MOV INT %s = %d\n", $2, $4);
          free($2);
      }
    | CHAR ID ASSIGN expr ';' {
          set_var($2, $4);
          printf("MOV CHAR %s = %d\n", $2, $4);
          free($2);
      }
    | FLOAT ID ASSIGN expr ';' {
          set_var($2, $4);
          printf("MOV FLOAT %s = %d\n", $2, $4);
          free($2);
      }
    | ID ASSIGN expr ';' {
          set_var($1, $3);
          printf("MOV %s = %d\n", $1, $3);
          free($1);
      }
    | PRINT '(' expr ')' ';' {
          printf("PRINT => %d\n", $3);
      }
    | IF '(' bool_expr ')' '{' program '}' {
          if ($3) {
              printf("IF TRUE BLOCK\n");
          } else {
              printf("IF SKIPPED\n");
          }
      }
    | IF '(' bool_expr ')' '{' program '}' ELSE '{' program '}' {
          if ($3) {
              printf("IF TRUE BLOCK\n");
          } else {
              printf("ELSE BLOCK\n");
          }
      }
    | WHILE '(' bool_expr ')' '{' program '}' {
          if ($3) {
              printf("WHILE BLOCK executed once (demo)\n");
          }
      }
    | FOR '(' ID ASSIGN expr ';' bool_expr ';' ID ASSIGN expr ')' '{' program '}' {
          set_var($3, $5);
          printf("FOR INIT: %s = %d\n", $3, $5);
          if ($7) {
              printf("FOR BODY (simulated once)\n");
              set_var($9, $11);
              printf("FOR UPDATE: %s = %d\n", $9, $11);
          }
          free($3); free($9);
      }
    | RETURN expr ';' {
          printf("RETURN => %d\n", $2);
      }
    ;

expr:
      expr '+' expr { $$ = $1 + $3; }
    | expr '-' expr { $$ = $1 - $3; }
    | expr '*' expr { $$ = $1 * $3; }
    | expr '/' expr { $$ = $1 / $3; }
    | '-' expr %prec UMINUS { $$ = -$2; }
    | '(' expr ')' { $$ = $2; }
    | NUMBER { $$ = $1; }
    | ID {
          $$ = get_var($1);
          free($1);
      }
    ;

bool_expr:
      expr EQ expr { $$ = $1 == $3; }
    | expr NEQ expr { $$ = $1 != $3; }
    | expr '<' expr { $$ = $1 < $3; }
    | expr '>' expr { $$ = $1 > $3; }
    | expr LE expr { $$ = $1 <= $3; }
    | expr GE expr { $$ = $1 >= $3; }
    ;

%%

int main() {
    printf("=== Mini C Compiler ===\n");
    return yyparse();
}

