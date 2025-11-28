#include "client.h"
#include "../common/utils.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// send the guess to the server
void send_guess(char letter){
  char msg[32];
  snprintf(msg, sizeof(msg), "GUESS %c", letter);
  (void)send(state.socket_fd, msg, strlen(msg), 0);
}

// parralell running to catch server messages
void *network_listen_thread(void *arg) {
  (void)arg;
  char buf[1024];

  while (state.running) {
    int n = read(state.socket_fd, buf, sizeof(buf) - 1);
    if (n <= 0) {
      pthread_mutex_lock(&state.state_mutex);
      strcpy(state.status_msg, "disconnected.");
      state.running = 0;
      pthread_mutex_unlock(&state.state_mutex);
      break;
    }

    buf[n] = '\0';
    pthread_mutex_lock(&state.state_mutex);

    char *ln = strtok(buf, "\n");
    while (ln) {
      if (strncmp(ln, "LEVEL", 5) == 0)
        sscanf(ln, "LEVEL %d", &state.level);

      else if (strncmp(ln, "WORD", 4) == 0)
        sscanf(ln, "WORD %s", state.masked_word);

      else if (strncmp(ln, "UPDATE", 6) == 0)
        sscanf(ln, "UPDATE %s %d", state.masked_word, &state.mistakes);

      else if (strncmp(ln, "TIMER", 5) == 0)
          sscanf(ln, "TIMER %d", state.timer_val);

      else if (strncmp(ln, "NEW_WORD", 8) == 0) {
        sscanf(ln, "NEW_WORD %s", state.masked_word);
        state.mistakes = 0;
      }

      else if (strncmp(ln, "WINNER", 6) == 0) {
        char winner[32] = {0};
        sscanf(ln, "WINNER %31s", winner);
        snprintf(state.status_msg, sizeof(state.status.msg),
          "winner: %s", winner);
        state.game_over = 1;
      }

      ln = strtok(NULL, "\n");
    }
    
    pthread_mutex_unlock(&state.state_mutex);
  }

  return NULL;
}   




