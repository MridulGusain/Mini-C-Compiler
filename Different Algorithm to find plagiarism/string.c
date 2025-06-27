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

int min(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

int levenshtein_distance(TokenList *list1, TokenList *list2) {
    int m = list1->count;
    int n = list2->count;
    int **dp = (int **)malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) {
        dp[i] = (int *)malloc((n + 1) * sizeof(int));
    }
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int cost = strcmp(list1->tokens[i - 1], list2->tokens[j - 1]) == 0 ? 0 : 1;
            dp[i][j] = min(
                dp[i - 1][j] + 1,
                dp[i][j - 1] + 1,
                dp[i - 1][j - 1] + cost
            );
        }
    }
    int dist = dp[m][n];
    for (int i = 0; i <= m; i++) free(dp[i]);
    free(dp);
    return dist;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <tokens1.txt> <tokens2.txt>\n", argv[0]);
        return 1;
    }
    TokenList list1, list2;
    read_tokens(argv[1], &list1);
    read_tokens(argv[2], &list2);
    int lev_dist = levenshtein_distance(&list1, &list2);
    printf("Levenshtein distance: %d\n", lev_dist);
    int max_len = list1.count > list2.count ? list1.count : list2.count;
    double similarity = max_len ? (1.0 - ((double)lev_dist / max_len)) * 100.0 : 100.0;
    printf("Levenshtein similarity: %.2f%%\n", similarity);
    return 0;
}
