#include "client.h"
#include "../common/utils.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void send_guess(char letter) {
    char buf[64];
    snprintf(buf, sizeof(buf), "GUESS %c\n", letter);
    send(state.socket_fd, buf, strlen(buf), 0);
}

void *network_listen_thread(void *arg) {
    (void)arg;

    char buffer[1024];

    while (state.running) {
        int n = read(state.socket_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            snprintf(state.status_msg, sizeof(state.status_msg),
                     "Disconnected from server.");
            state.running = 0;
            break;
        }

        buffer[n] = '\0';

        pthread_mutex_lock(&state.state_mutex);

        char *line = strtok(buffer, "\n");
        while (line) {

            if (strncmp(line, "LEVEL", 5) == 0) {
                sscanf(line, "LEVEL %d", &state.level);

            } else if (strncmp(line, "WORD", 4) == 0) {
                sscanf(line, "WORD %63s", state.masked_word);

            } else if (strncmp(line, "UPDATE", 6) == 0) {
                sscanf(line, "UPDATE %63s %d",
                       state.masked_word, &state.mistakes);

            } else if (strncmp(line, "TIMER", 5) == 0) {
                sscanf(line, "TIMER %d", &state.timer_val);

            } else if (strncmp(line, "TIME_UP", 7) == 0) {
                snprintf(state.status_msg, sizeof(state.status_msg),
                         "Time's up! New word assigned.");

            } else if (strncmp(line, "NEW_WORD", 8) == 0) {
                sscanf(line, "NEW_WORD %63s", state.masked_word);
                state.mistakes = 0;

            } else if (strncmp(line, "GUESSED", 7) == 0) {
                sscanf(line, "GUESSED %26s", state.guessed_letters);
                
            } else if (strncmp(line, "ALREADY_GUESSED", 15) == 0) {
                char letter;
                sscanf(line, "ALREADY_GUESSED %c", &letter);
                snprintf(state.status_msg, sizeof(state.status_msg), "Letter '%c' already guessed.", letter);
                
            } else if (strncmp(line, "WORD_LEN", 8) == 0) { 
                sscanf(line, "WORD_LEN %d", &state.word_len);

            } else if (strncmp(line, "GAME_OVER", 9) == 0) {
                snprintf(state.status_msg, sizeof(state.status_msg), "Game Over! The word was: %s", state.current_word);
                state.game_over = 1;
            }
            // ------------------------------------------------------------------
            else if (strncmp(line, "WINNER", 6) == 0) {
                char name[32];
                sscanf(line, "WINNER %31s", name);
                snprintf(state.status_msg, sizeof(state.status_msg),
                         "Winner: %s", name);
                state.game_over = 1;
            }

            line = strtok(NULL, "\n");
        }

        pthread_mutex_unlock(&state.state_mutex);
    }

    return NULL;
}

