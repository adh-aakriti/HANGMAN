#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <pthread.h>
#include "../common/protocol.h"

int create_server_socket(int port);

typedef struct {
    int fd;
    struct sockaddr_in addr;
    int id;
    int active;
    
    // Game State
    int level;
    char word[64];
    char mask[64];
    int errors;
    time_t start;
    int limit;
    char guessed[27];
    int len;
    int done;
} Client;

// Global State
extern Client *clients[100];
extern int count;
extern pthread_mutex_t mutex;

// Functions
void init_words();
char* get_random_word(int length);
void update_leaderboard(int client_id, const char* result);
void broadcast(const char* message);

#endif
