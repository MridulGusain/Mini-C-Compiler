#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100

int is_keyword(const char *str) {
    const char *keywords[] = {"int", "float", "return", "if", "else", "while", "for", "void", NULL};
    for (int i = 0; keywords[i]; i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source_code.c> <tokens.txt>\n", argv[0]);
        return 1;
    }
    FILE *src = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    if (!src || !out) {
        perror("Error opening file");
        return 1;
    }
    int ch;
    char token[MAX_TOKEN_LEN];
    while ((ch = fgetc(src)) != EOF) {
        if (isspace(ch)) continue;
        if (isalpha(ch) || ch == '_') {
            int i = 0;
            token[i++] = ch;
            while ((ch = fgetc(src)) != EOF && (isalnum(ch) || ch == '_')) {
                if (i < MAX_TOKEN_LEN - 1) token[i++] = ch;
            }
            token[i] = '\0';
            if (is_keyword(token))
                fprintf(out, "%s ", token);
            else
                fprintf(out, "ID ");
            if (ch != EOF) ungetc(ch, src);
        } else if (isdigit(ch)) {
            int i = 0;
            token[i++] = ch;
            while ((ch = fgetc(src)) != EOF && isdigit(ch)) {
                if (i < MAX_TOKEN_LEN - 1) token[i++] = ch;
            }
            token[i] = '\0';
            fprintf(out, "NUM ");
            if (ch != EOF) ungetc(ch, src);
        } else if (ch == '=') {
            fprintf(out, "ASSIGN ");
        } else if (ch == ';') {
            fprintf(out, "SEMI ");
        } else if (ch == '+') {
            fprintf(out, "PLUS ");
        } else if (ch == '*') {
            fprintf(out, "MUL ");
        } // Add more operators as needed
        // Ignore other characters
    }
    fclose(src);
    fclose(out);
    return 0;
}
