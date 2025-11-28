#include "client.h"
#include "../common/utils.h"

#include <stdio.h>

static void draw_text(SDL_Renderer *ren, TTF_Font *font,
                      const char *txt, int x, int y) {
    if (!font || !txt) return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Solid(font, txt, white);
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

void render_game(SDL_Renderer *renderer, TTF_Font *font) {

    SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
    SDL_RenderClear(renderer);

    pthread_mutex_lock(&state.state_mutex);

    char top_line[128];
    snprintf(top_line, sizeof(top_line),
             "Level: %d   Time: %d   Mistakes: %d",
             state.level, state.timer_val, state.mistakes);
    draw_text(renderer, font, top_line, 20, 20);

    draw_text(renderer, font, state.masked_word, 20, 80);

    draw_text(renderer, font, state.status_msg, 20, 140);

    pthread_mutex_unlock(&state.state_mutex);

    SDL_RenderPresent(renderer);
}
