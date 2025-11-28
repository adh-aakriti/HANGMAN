#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include "../common/protocol.h"

int create_server_socket(int port);

typedef struct {
    int socket_fd;
    struct sockaddr_in address;
    int id;
    int active;
    
    int level;
    char current_word[64];
    char masked_word[64];
    int mistakes;
    time_t start_time;
    int time_limit;
    char guessed_letters[27];
    int word_len;
    int finished;
} Client;

extern Client *clients[100];
extern int client_count;
extern pthread_mutex_t clients_mutex;

void init_words();
char* get_random_word(int length);
void update_leaderboard(int client_id, const char* result);
void broadcast(const char* message);

#endif
