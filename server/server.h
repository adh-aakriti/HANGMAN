#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <pthread.h>
#include "../common/protocol.h"

int create_server_socket(int port);

typedef struct {
    int socket_fd;
    struct sockaddr_in address;
    int id;
    int active;
    
    // Game State
    int level;
    char current_word[64];
    char masked_word[64];
    int mistakes;
    time_t start_time;
    int time_limit;
    int finished;
} Client;

// Global State
extern Client *clients[100];
extern int client_count;
extern pthread_mutex_t clients_mutex;

// Functions
void init_words();
char* get_random_word(int length);
void update_leaderboard(int client_id, const char* result);
void broadcast(const char* message);

#endif

