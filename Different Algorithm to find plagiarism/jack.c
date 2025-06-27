#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 10000
#define MAX_TOKEN_LEN 32

typedef struct {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int count;
} TokenList;

void read_tokens(const char *filename, TokenList *list) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening token file");
        exit(1);
    }
    list->count = 0;
    while (fscanf(file, "%31s", list->tokens[list->count]) == 1) {
        list->count++;
        if (list->count >= MAX_TOKENS) break;
    }
    fclose(file);
}

int token_in_list(const char *token, TokenList *list) {
    for (int i = 0; i < list->count; i++) {
        if (strcmp(token, list->tokens[i]) == 0) return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <tokens1.txt> <tokens2.txt>\n", argv[0]);
        return 1;
    }
    TokenList list1, list2;
    read_tokens(argv[1], &list1);
    read_tokens(argv[2], &list2);

    int intersection = 0, union_count = 0;
    // Count unique tokens in both lists (union)
    TokenList union_list = { .count = 0 };
    for (int i = 0; i < list1.count; i++) {
        if (!token_in_list(list1.tokens[i], &union_list)) {
            strcpy(union_list.tokens[union_list.count++], list1.tokens[i]);
        }
    }
    for (int i = 0; i < list2.count; i++) {
        if (!token_in_list(list2.tokens[i], &union_list)) {
            strcpy(union_list.tokens[union_list.count++], list2.tokens[i]);
        }
    }
    union_count = union_list.count;
    // Count intersection
    for (int i = 0; i < union_list.count; i++) {
        if (token_in_list(union_list.tokens[i], &list1) && token_in_list(union_list.tokens[i], &list2)) {
            intersection++;
        }
    }
    double similarity = union_count ? ((double)intersection / union_count) : 0.0;
    printf("Similarity: %.2f%%\n", similarity * 100);
    if (similarity > 0.8) {
        printf("Warning: High similarity detected! Possible plagiarism.\n");
    }
    return 0;
}
