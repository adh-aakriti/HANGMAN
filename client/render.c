#include "client.h"
#include "../common/utils.h"
#include <stdio.h>
#include <string.h>

void render_text(SDL_Renderer *ren, TTF_Font *font,
                 const char *txt, int x, int y, SDL_Color color) {
    if (!font || !txt) return;

    SDL_Surface *surf = TTF_RenderText_Solid(font, txt, color);
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    if (!tex) return;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
}

static void render_centered_text(SDL_Renderer *ren, TTF_Font *font,
                                 const char *txt, int y, SDL_Color color) {
    int w, h;
    TTF_SizeText(font, txt, &w, &h);
    render_text(ren, font, txt, (800 - w) / 2, y, color);
}


static char* format_word_display(const char *masked_word, int len, char *buffer) {
    buffer[0] = '\0';
    for (int i = 0; i < len; i++) {
        char temp[3] = {masked_word[i], ' ', '\0'};
        strcat(buffer, temp);
    }
    return buffer;
}


void render_game(SDL_Renderer *renderer, TTF_Font *font) {
    
    SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color white = {255, 255, 255, 255};

    pthread_mutex_lock(&state.state_mutex);

    char top_line[128];
    snprintf(top_line, sizeof(top_line),
             "Level: %d   Time: %d   Mistakes: %d",
             state.level, state.timer_val, state.mistakes);
    render_text(renderer, font, top_line, 20, 20, white);

    char word_display_buf[128];
    format_word_display(state.masked_word, state.word_len, word_display_buf);
    render_centered_text(renderer, font, word_display_buf, 100, white); 

    render_centered_text(renderer, font, state.status_msg, 160, white); 


    pthread_mutex_unlock(&state.state_mutex);

    SDL_RenderPresent(renderer);
}

