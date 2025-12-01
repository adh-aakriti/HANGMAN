#include "client.h"
#include "../common/utils.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

extern GameState state; 

void send_guess(char c) {
    char buf[64];
    snprintf(buf, sizeof(buf), "GUESS %c\n", c);
    send(state.fd, buf, strlen(buf), 0);
}

void *network_listen_thread(void *arg) {
    (void)arg;

    char buf[1024];

while (state.running) {
    int n = read(state.fd, buf, sizeof(buf) - 1);
        if (n <= 0) {
            if (!state.over) {
                snprintf(state.msg, sizeof(state.msg),
                         "Disconnected from server.");
                state.running = 0;
            }
            break;
        }

        buf[n] = '\0';

        pthread_mutex_lock(&state.mutex);

        char *line = strtok(buf, "\n");
        while (line) {

            if (strncmp(line, "LEVEL", 5) == 0) {
                sscanf(line, "LEVEL %d", &state.level);
                snprintf(state.msg, sizeof(state.msg),
                     "Level %d", state.level);

            } else if (strncmp(line, "WORD_LEN", 8) == 0) {
                sscanf(line, "WORD_LEN %d", &state.len);

            } else if (strncmp(line, "WORD", 4) == 0) {
                sscanf(line, "WORD %63s", state.mask);
                state.len = (int)strlen(state.mask);
                state.errors = 0; 

            } else if (strncmp(line, "UPDATE", 6) == 0) {
                sscanf(line, "UPDATE %63s %d",
                       state.mask, &state.errors);
                state.len = (int)strlen(state.mask);

            } else if (strncmp(line, "TIMER", 5) == 0) {
                int t;
                sscanf(line, "TIMER %d", &t);
                state.timer = t;
                if (state.time == 0) {   
                    state.time = t;
                }

            } else if (strncmp(line, "TIME_UP", 7) == 0) {
                snprintf(state.msg, sizeof(state.msg),
                         "Time's up! New word assigned.");

            } else if (strncmp(line, "NEW_WORD", 8) == 0) {
                sscanf(line, "NEW_WORD %63s", state.mask);
                state.errors = 0;
                state.len = (int)strlen(state.mask);
            
            } else if (strncmp(line, "GUESSED", 7) == 0) {
                state.guessed[0] = '\0';
                sscanf(line, "GUESSED %26s", state.guessed);
                char tmp[27];
                if (sscanf(line, "GUESSED %26s", tmp) == 1) {
                    strncpy(state.guessed, tmp, 26);
                    state.guessed[26] = '\0';
                }

            } else if (strncmp(line, "ALREADY_GUESSED", 15) == 0) {
                char c;
                sscanf(line, "ALREADY_GUESSED %c", &c);
                snprintf(state.msg, sizeof(state.msg), "Letter '%c' already guessed.", c);

            } else if (strncmp(line, "FINAL_WORD", 10) == 0) {
                sscanf(line, "FINAL_WORD %63s", state.word);

            } else if (strncmp(line, "GAME_OVER", 9) == 0) {
                char result[16];
                sscanf(line, "GAME_OVER %15s", result);
                if (strcmp(result, "LOSE") == 0) {
                    snprintf(state.msg, sizeof(state.msg), 
                             "Game Over! The word was: %s", state.word);
                    state.win = 0;
                }
                state.over = 1;

            } else if (strncmp(line, "WINNER", 6) == 0) {
                char name[32];
                sscanf(line, "WINNER %31s", name);
                snprintf(state.msg, sizeof(state.msg),
                         "Winner: %s", name);
                state.win = 1;
                state.over = 1;
            }

            line = strtok(NULL, "\n");
        }

        pthread_mutex_unlock(&state.mutex);
    }

    return NULL;
}









