#include "server.h"
#include "../common/utils.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

#define MAX_WORDS 100

char w4[MAX_WORDS][64];
char w3[MAX_WORDS][64];
char w2[MAX_WORDS][64];
int n4 = 0, n3 = 0, n2 = 0;

// loads words from txt
void load_words(const char* file, char list[][64], int *n) {
    FILE *f = fopen(file, "r");
    if (!f) return;

    while (*n < MAX_WORDS && fgets(list[*n], 64, f)) {
        trim_newline(list[*n]);
        (*n)++;
    }
    fclose(f);
}

void init_words() {
    srand(time(NULL));
    int a = 0, b = 0, c = 0;

    load_words("../assets/words_4.txt", w4, &a);
    load_words("../assets/words_3.txt", w3, &b);
    load_words("../assets/words_2.txt", w2, &c);

    n4 = a;
    n3 = b;
    n2 = c;
}

char* get_random_word(int length) {
    if (length == 4 && n4 > 0) return w4[rand() % n4];
    if (length == 3 && n3 > 0) return w3[rand() % n3];
    if (length == 2 && n2 > 0) return w2[rand() % n2];
    return "ok";

}





