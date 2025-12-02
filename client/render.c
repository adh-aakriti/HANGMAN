#include "client.h"
#include "../common/utils.h"
#include <stdio.h>
#include <string.h>
extern GameState state;

void render_text(SDL_Renderer *ren, TTF_Font *font,
    const char *txt, int x, int y, SDL_Color col) {
  
      if (!font || !txt) {
        return;
      }
      SDL_Surface *surf = TTF_RenderText_Solid(font, txt, col);
  
      if (!surf) {
        return;
      }
      SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
      SDL_FreeSurface(surf);
  
      if (!tex) {
        return;
      }
      SDL_Rect dst;
      dst.x = x;
      dst.y = y;
      SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
      SDL_RenderCopy(ren, tex, NULL, &dst);
      SDL_DestroyTexture(tex);
}
static void render_centered_text(SDL_Renderer *ren, TTF_Font *font, const char *txt, int y, SDL_Color col) {
    int w, h;
    TTF_SizeText(font, txt, &w, &h);
    render_text(ren, font, txt, (800 - w) / 2, y, col);
}

static char* format_word_display(const char *mask, int len, char *buf) {
    buf[0] = '\0';
  
    for (int i = 0; i < len; i++) {
        char tmp[4] = {mask[i], ' ',' ', '\0'};
        strcat(buf, tmp);
    }
    return buf;
}
static void render_hangman(SDL_Renderer *ren, int errors) {
    int x = 100;
    int y = 400; 
    int h = 200;
    int head = 15;
    int body = 40;
    int arm = 20;
    int leg = 25;
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); 
    SDL_RenderDrawLine(ren, x - 50, y, x + 50, y); 
    SDL_RenderDrawLine(ren, x, y, x, y - h); 
    SDL_RenderDrawLine(ren, x, y - h, x + 70, y - h); 
    int rx = x + 70;
    int ry = y - h;
    int cy = ry + head * 2;
    SDL_RenderDrawLine(ren, rx, ry, rx, cy - head);

    if (errors >= 1) {
        SDL_Rect r = {rx - head, cy - head, head * 2, head * 2};
        SDL_RenderDrawRect(ren, &r);
    }
    int top = cy + head;
    int bot = top + body;
  
    if (errors >= 2) {
        SDL_RenderDrawLine(ren, rx, top, rx, bot);
    }
    int sh = top + 5;
  
    if (errors >= 3) {
        SDL_RenderDrawLine(ren, rx, sh, rx - arm, sh + arm);
    }
  
    if (errors >= 4) {
        SDL_RenderDrawLine(ren, rx, sh, rx + arm, sh + arm);
    }
    
    if (errors >= 5) {
        SDL_RenderDrawLine(ren, rx, bot, rx - leg, bot + leg);
    }
    
    if (errors >= 6) {
        SDL_RenderDrawLine(ren, rx, bot, rx + leg, bot + leg);
    }
    
    if (errors >= 7) {
        int off = head / 3;
        SDL_RenderDrawLine(ren, rx - off, cy - off, rx - head + off, cy + off);
        SDL_RenderDrawLine(ren, rx - off, cy + off, rx - head + off, cy - off);
        SDL_RenderDrawLine(ren, rx + head - off, cy - off, rx + off, cy + off);
        SDL_RenderDrawLine(ren, rx + head - off, cy + off, rx + off, cy - off);
    }
}

static void render_keyboard(SDL_Renderer *ren, TTF_Font *font) {
    int sx = 100; 
    int sy = 450;
    int kw = 40;
    int kh = 40;
    int sp = 5;
    const char *rows[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    int cnt = sizeof(rows) / sizeof(rows[0]);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {0, 150, 0, 255};
    SDL_Color red = {150, 0, 0, 255};
    SDL_Color gray = {100, 100, 100, 255};
    pthread_mutex_lock(&state.mutex);

    for (int r = 0; r < cnt; r++) {
        int off = (r == 0) ? 0 : (r == 1) ? (kw + sp) * 0.5 : (kw + sp) * 1.5; 
        
        for (int i = 0; rows[r][i] != '\0'; i++) {
            char c = rows[r][i];
            SDL_Color col = gray; 
            
            if (strchr(state.guessed, c)) {
                
                if (strchr(state.mask, c)) {
                    col = green;
                } else {
                    col = red;
                }
            }
            SDL_Rect rect = {
                sx + off + i * (kw + sp), 
                sy + r * (kh + sp), 
                kw, 
                kh
            };
            SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
            SDL_RenderFillRect(ren, &rect);
            char str[2] = {c, '\0'};
            render_text(ren, font, str, rect.x + 10, rect.y + 10, white);
        }
    }
    pthread_mutex_unlock(&state.mutex);
}
void render_game(SDL_Renderer *ren, TTF_Font *font) {
    SDL_SetRenderDrawColor(ren, 20, 20, 25, 255);
    SDL_RenderClear(ren);
    SDL_Color white = {255, 255, 255, 255};
    pthread_mutex_lock(&state.mutex);
  
    if (state.over) {
      int used = 0;
      
      if (state.time > 0 && state.timer >= 0) {
          used = state.time - state.timer;
        
          if (used < 0){
            used = 0;
          }
      }
  
      if (state.win) {
          char line[128];
          snprintf(line, sizeof(line), "Congrats! You won!");
          render_centered_text(ren, font, line, 200, white);
          snprintf(line, sizeof(line), "Your time was: %d seconds", used);
          render_centered_text(ren, font, line, 260, white); 
      } 
      
      else {
          render_centered_text(ren, font, "Game Over", 220, white);
          render_centered_text(ren, font, "Better luck next time!", 260, white);
      }
      render_centered_text(ren, font, "Press ESC or close the window to exit", 320, white);
      pthread_mutex_unlock(&state.mutex);
      SDL_RenderPresent(ren);
      return;
  }
    char top[128];
    snprintf(top, sizeof(top), "Level: %d   Time: %d   Mistakes: %d/7", state.level, state.timer, state.errors);
    render_centered_text(ren, font, top, 80, white);
    char buf[128];
    format_word_display(state.mask, state.len, buf);
    render_centered_text(ren, font, buf, 300, white); 
    render_centered_text(ren, font, state.msg, 160, white); 
    render_hangman(ren, state.errors);
    pthread_mutex_unlock(&state.mutex);
    render_keyboard(ren, font); 
    SDL_RenderPresent(ren);
}
