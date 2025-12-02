#include "client.h"
#include "../common/utils.h"
#include <SDL.h>
#include <SDL_ttf.h>

SDL_Window* init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0){ //loads sdl core 
        die("SDL Init failed, check SDL install maybe?");
    }

    if (TTF_Init() != 0){
        die("Font system didn't start, TTF_Init error");
    }
    SDL_Window *win = SDL_CreateWindow("Hangman Race", 100, 100, 800, 600, SDL_WINDOW_SHOWN); //creates window
    
    if (!win){
        die("Window didn't spawn, SDL_CreateWindow error");
    }
    return win;
}
void cleanup_sdl(SDL_Window* win, SDL_Renderer* ren){
    if (ren) {
        SDL_DestroyRenderer(ren);
    }
    if (win){ 
        SDL_DestroyWindow(win);
    }
    TTF_Quit();
    SDL_Quit();
}







