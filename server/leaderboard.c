#include "server.h"
#include <stdio.h>

void update_leaderboard(int client_id, const char* result) {
    FILE *f = fopen("leaderboard.txt", "a");
    if (f) {
        fprintf(f, "Player %d: %s\n", client_id, result);
        fclose(f);
    }
}
