#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 10000
#define MAX_TOKEN_LEN 32
#define MAX_FINGERPRINTS 10000
#define K_GRAM 5  // You can adjust k as needed

typedef struct {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int count;
} TokenList;

typedef struct {
    unsigned int hashes[MAX_FINGERPRINTS];
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
            hash = ((hash << 5) + hash) + kgram[i][j]; // hash * 33 + c
        }
    }
    return hash;
}

void generate_fingerprints(TokenList *list, FingerprintSet *fp_set) {
    fp_set->count = 0;
    char kgram[K_GRAM][MAX_TOKEN_LEN];
    for (int i = 0; i <= list->count - K_GRAM; i++) {
        for (int j = 0; j < K_GRAM; j++) {
            strcpy(kgram[j], list->tokens[i + j]);
        }
        unsigned int h = hash_kgram(kgram);
        // Avoid duplicates
        int duplicate = 0;
        for (int k = 0; k < fp_set->count; k++) {
            if (fp_set->hashes[k] == h) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate && fp_set->count < MAX_FINGERPRINTS) {
            fp_set->hashes[fp_set->count++] = h;
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
    generate_fingerprints(&list1, &fp1);
    generate_fingerprints(&list2, &fp2);
    // Jaccard similarity of fingerprints
    int intersection = 0, union_count = fp1.count;
    for (int i = 0; i < fp2.count; i++) {
        if (hash_in_set(fp2.hashes[i], &fp1)) {
            intersection++;
        } else {
            union_count++;
        }
    }
    double similarity = union_count ? ((double)intersection / union_count) * 100.0 : 100.0;
    printf("Fingerprint : %.2f%%\n", similarity);
    return 0;
}
