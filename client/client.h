#ifndef CLIENT_H
#define CLIENT_H


#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>
#include "../common/protocol.h"

typedef struct {
    int socket_fd;
    int running;

    // core game values we sync with server
    int level;
    char masked_word[64];
    int mistakes;
    int timer_val;
    char status_msg[128];
    int game_over;
    char guessed_letters[27];
    int word_len;
    char current_word[64];

    pthread_mutex_t state_mutex;
} GameState;

extern GameState state;
void render_text(SDL_Renderer *ren, TTF_Font *font, const char *txt, int x, int y, SDL_Color color);

// SDL setup / cleanup
SDL_Window* init_sdl();
void cleanup_sdl(SDL_Window* win, SDL_Renderer* ren);

// draws game screen
void render_game(SDL_Renderer *renderer, TTF_Font *font);

// network
void *network_listen_thread(void *arg);
void send_guess(char letter);

#endif








