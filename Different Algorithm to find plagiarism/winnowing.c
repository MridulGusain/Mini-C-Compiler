#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 10000
#define MAX_TOKEN_LEN 32
#define MAX_HASHES 10000
#define K_GRAM 5      // Length of k-gram
#define WINDOW 4      // Window size for winnowing (can be K_GRAM or K_GRAM+1)

typedef struct {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int count;
} TokenList;

typedef struct {
    unsigned int hashes[MAX_HASHES];
    int count;
} FingerprintSet;

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

unsigned int hash_kgram(char kgram[K_GRAM][MAX_TOKEN_LEN]) {
    unsigned int hash = 5381;
    for (int i = 0; i < K_GRAM; i++) {
        for (int j = 0; kgram[i][j] != '\0'; j++) {
            hash = ((hash << 5) + hash) + kgram[i][j];
        }
    }
    return hash;
}

void winnowing(TokenList *list, FingerprintSet *fp_set) {
    unsigned int hashes[MAX_TOKENS] = {0};
    int num_hashes = 0;
    char kgram[K_GRAM][MAX_TOKEN_LEN];
    for (int i = 0; i <= list->count - K_GRAM; i++) {
        for (int j = 0; j < K_GRAM; j++) {
            strcpy(kgram[j], list->tokens[i + j]);
        }
        hashes[num_hashes++] = hash_kgram(kgram);
    }
    fp_set->count = 0;
    int last_pos = -1;
    unsigned int last_min = 0;
    for (int i = 0; i <= num_hashes - WINDOW; i++) {
        unsigned int min = hashes[i];
        int min_pos = i;
        for (int j = 1; j < WINDOW; j++) {
            if (hashes[i + j] <= min) {
                min = hashes[i + j];
                min_pos = i + j;
            }
        }
        if (min_pos != last_pos || min != last_min) {
            fp_set->hashes[fp_set->count++] = min;
            last_pos = min_pos;
            last_min = min;
            if (fp_set->count >= MAX_HASHES) break;
        }
    }
}

int hash_in_set(unsigned int h, FingerprintSet *set) {
    for (int i = 0; i < set->count; i++) {
        if (set->hashes[i] == h) return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <tokens1.txt> <tokens2.txt>\n", argv[0]);
        return 1;
    }
    TokenList list1, list2;
    FingerprintSet fp1, fp2;
    read_tokens(argv[1], &list1);
    read_tokens(argv[2], &list2);
    winnowing(&list1, &fp1);
    winnowing(&list2, &fp2);
    
    int intersection = 0, union_count = fp1.count;
    for (int i = 0; i < fp2.count; i++) {
        if (hash_in_set(fp2.hashes[i], &fp1)) {
            intersection++;
        } else {
            union_count++;
        }
    }
    double similarity = union_count ? ((double)intersection / union_count) * 100.0 : 100.0;
    printf("Winnowing similarity: %.2f%%\n", similarity);
    return 0;
}
