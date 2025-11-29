#ifndef CLIENT_H
#define CLIENT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>
#include "../common/protocol.h"

typedef struct {
    int socket_fd;
    int running;

    int level;
    char masked_word[64];
    int mistakes;
    int timer_val;
    char status_msg[128];
    int game_over;
    
    char guessed_letters[27]; 
    int word_len;             
    char current_word[64];

    Uint32 start_ticks;
    int total_time; 
    int win;

    pthread_mutex_t state_mutex;
} GameState;

extern GameState state;

SDL_Window* init_sdl();
void cleanup_sdl(SDL_Window* win, SDL_Renderer* ren);

void render_game(SDL_Renderer *renderer, TTF_Font *font);

void *network_listen_thread(void *arg);
void send_guess(char letter);

#endif


