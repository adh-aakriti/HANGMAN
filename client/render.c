#include "client.h"
void draw_hangman(SDL_Renderer *ren, int mistakes) {
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

  // To Draw The Base
  if (mistakes >= 0) {
    SDL_RenderDrawLine(ren, 50, 400, 250, 400); 
  }
  // To Draw The Pole
  if (mistakes >= 1) { 
    SDL_RenderDrawLine(ren, 150, 400, 150, 100); 
  }
  // To Draw The Top
  if (mistakes >= 2) { 
    SDL_RenderDrawLine(ren, 150, 100, 300, 100); 
  }
  // To Draw The Rope
  if (mistakes >= 3) {
    SDL_RenderDrawLine(ren, 300, 100, 300, 150); 
  }
  // To Draw The Head
  if (mistakes >= 4) {
      SDL_Rect head;
      head.x = 280;
      head.y = 150;
      head.w = 40;
      head.h = 40;
      SDL_RenderDrawRect(ren, &head);
  }
  // To Draw The Body
  if (mistakes >= 5) { 
    SDL_RenderDrawLine (ren, 300, 190, 300, 300); 
  }
  // To Draw The Arms/Legs 
  if (mistakes >= 6) {
      SDL_RenderDrawLine(ren, 300, 220, 270, 350);
      SDL_RenderDrawLine(ren, 300, 220, 330, 250);
      SDL_RenderDrawLine(ren, 300, 300, 270, 350);
      SDL_RenderDrawLine(ren, 300, 300, 330, 350);
  }
}

void render_text(SDL_Renderer *ren, TTF_Font *font, const char *text, int x, int y) {
    if (!text || strlen(text) == 0) return;
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderTextSolid(font, text, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void render_game(SDL_Renderer *renderer, TTF_Font *font) {
    pthread_mutex_lock(&state_mutex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    char buf[128]

    // Info Header
    snprintf(buf, sizeof(buf), "Level: %d/3   Time: %d", state.level, state.timer_val);
    render_text(renderer, font, buf, 20, 20);

    // Status Message
    render_text(renderer, font, state.status_msg, 20, 500);

    // Word
    render_text(renderer, font, state.masked_word, 400, 250);

    // Hangman
    draw_hangman(renderer, state.mistakes);

    pthread_mutex_unlock(&state.state_mutex);

    SDL_RenderPresent(renderer);
}



