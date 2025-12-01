#include "server.h"
#include <stdio.h>

void update_leaderboard(int id, const char* result) {
    FILE *f = fopen("leaderboard.txt", "a");
    if (f) {
        fprintf(f, "Player %d: %s\n", id, result);
        fclose(f);
    }
}
