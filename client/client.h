#ifndef CLIENT_H //another check to make sure no repetitiveness
#define CLIENT_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>
#include "../common/protocol.h"

typedef struct {
    int fd;
    int running;
    int level;
    char mask[64];
    int errors;
    int timer;
    char msg[128];
    int over;
    char guessed[27]; 
    int len;             
    char word[64];
    Uint32 start;
    int time; 
    int win;
    pthread_mutex_t mutex;
} 
GameState;

extern GameState state;
SDL_Window* init_sdl(); //initializes sdl and returns window pointer
void cleanup_sdl(SDL_Window* win, SDL_Renderer* ren); //stops sdl and ttf
void render_game(SDL_Renderer *renderer, TTF_Font *font); //draws current game frame
void *network_listen_thread(void *arg); //listen to server message 
void send_guess(char letter); //send single char guess over socket
#endif

