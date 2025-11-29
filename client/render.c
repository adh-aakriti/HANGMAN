#include "client.h"
#include "../common/utils.h"
#include <stdio.h>
#include <string.h>

extern GameState state;

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
        char temp[4] = {masked_word[i], ' ',' ', '\0'};
        strcat(buffer, temp);
    }
    return buffer;
}

static void render_hangman(SDL_Renderer *ren, int mistakes) {
    int gallows_x = 100;
    int gallows_y = 400; 
    int post_height = 200;
    int head_radius = 15;
    int body_length = 40;
    int arm_length = 20;
    int leg_length = 25;
    
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); 


    SDL_RenderDrawLine(ren, gallows_x - 50, gallows_y, gallows_x + 50, gallows_y); 
    
    SDL_RenderDrawLine(ren, gallows_x, gallows_y, gallows_x, gallows_y - post_height); 
    
    SDL_RenderDrawLine(ren, gallows_x, gallows_y - post_height, gallows_x + 70, gallows_y - post_height); 
    
    int rope_x = gallows_x + 70;
    int rope_top_y = gallows_y - post_height;
    int head_center_y = rope_top_y + head_radius * 2;
    SDL_RenderDrawLine(ren, rope_x, rope_top_y, rope_x, head_center_y - head_radius);

    if (mistakes >= 1) {
        SDL_Rect head = {rope_x - head_radius, head_center_y - head_radius, head_radius * 2, head_radius * 2};
        SDL_RenderDrawRect(ren, &head);
    }
    
    int body_top_y = head_center_y + head_radius;
    int body_bottom_y = body_top_y + body_length;
    if (mistakes >= 2) {
        SDL_RenderDrawLine(ren, rope_x, body_top_y, rope_x, body_bottom_y);
    }
    
    int shoulder_y = body_top_y + 5;
    if (mistakes >= 3) {
        SDL_RenderDrawLine(ren, rope_x, shoulder_y, rope_x - arm_length, shoulder_y + arm_length);
    }
    
    if (mistakes >= 4) {
        SDL_RenderDrawLine(ren, rope_x, shoulder_y, rope_x + arm_length, shoulder_y + arm_length);
    }
    
    if (mistakes >= 5) {
        SDL_RenderDrawLine(ren, rope_x, body_bottom_y, rope_x - leg_length, body_bottom_y + leg_length);
    }
    
    if (mistakes >= 6) {
        SDL_RenderDrawLine(ren, rope_x, body_bottom_y, rope_x + leg_length, body_bottom_y + leg_length);
    }
    
    if (mistakes >= 7) {
        int eye_offset = head_radius / 3;
        SDL_RenderDrawLine(ren, rope_x - eye_offset, head_center_y - eye_offset, rope_x - head_radius + eye_offset, head_center_y + eye_offset);
        SDL_RenderDrawLine(ren, rope_x - eye_offset, head_center_y + eye_offset, rope_x - head_radius + eye_offset, head_center_y - eye_offset);
        SDL_RenderDrawLine(ren, rope_x + head_radius - eye_offset, head_center_y - eye_offset, rope_x + eye_offset, head_center_y + eye_offset);
        SDL_RenderDrawLine(ren, rope_x + head_radius - eye_offset, head_center_y + eye_offset, rope_x + eye_offset, head_center_y - eye_offset);
    }
}

static void render_keyboard(SDL_Renderer *ren, TTF_Font *font) {
    int start_x = 100; 
    int start_y = 450;
    int key_w = 40;
    int key_h = 40;
    int spacing = 5;

    const char *rows[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    int row_count = sizeof(rows) / sizeof(rows[0]);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color correct_green = {0, 150, 0, 255};
    SDL_Color incorrect_red = {150, 0, 0, 255};
    SDL_Color unguessed_gray = {100, 100, 100, 255};

    pthread_mutex_lock(&state.state_mutex);

    for (int r = 0; r < row_count; r++) {
        int row_offset_x = (r == 0) ? 0 : (r == 1) ? (key_w + spacing) * 0.5 : (key_w + spacing) * 1.5; 
        
        for (int i = 0; rows[r][i] != '\0'; i++) {
            char letter = rows[r][i];
            
            SDL_Color key_color = unguessed_gray; 
            
            if (strchr(state.guessed_letters, letter)) {
                
                if (strchr(state.masked_word, letter)) {
                    key_color = correct_green;
                } else {
                    key_color = incorrect_red;
                }
            }

            SDL_Rect rect = {
                start_x + row_offset_x + i * (key_w + spacing), 
                start_y + r * (key_h + spacing), 
                key_w, 
                key_h
            };
            SDL_SetRenderDrawColor(ren, key_color.r, key_color.g, key_color.b, key_color.a);
            SDL_RenderFillRect(ren, &rect);
            
            char str[2] = {letter, '\0'};
            render_text(ren, font, str, rect.x + 10, rect.y + 10, white);
        }
    }

    pthread_mutex_unlock(&state.state_mutex);
}


void render_game(SDL_Renderer *renderer, TTF_Font *font) {

    SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color white = {255, 255, 255, 255};

    pthread_mutex_lock(&state.state_mutex);

    if (state.game_over) {
      int used_time = 0;
      if (state.total_time > 0 && state.timer_val >= 0) {
          used_time = state.total_time - state.timer_val;
          if (used_time < 0) used_time = 0;
      }
  
      if (state.win) {
          char line[128];
  
          snprintf(line, sizeof(line), "Congrats! You won!");
          render_centered_text(renderer, font, line, 200, white);
  
          snprintf(line, sizeof(line), "Your time was: %d seconds", used_time);
          render_centered_text(renderer, font, line, 260, white);
      } else {
          render_centered_text(renderer, font, "Game Over", 220, white);
          render_centered_text(renderer, font, "Better luck next time!", 260, white);
      }
  
      render_centered_text(renderer, font,
                           "Press ESC or close the window to exit",
                           320, white);
  
      pthread_mutex_unlock(&state.state_mutex);
      SDL_RenderPresent(renderer);
      return;
  }

    char top_line[128];
    snprintf(top_line, sizeof(top_line),
             "Level: %d   Time: %d   Mistakes: %d/7", 
             state.level, state.timer_val, state.mistakes);
    render_centered_text(renderer, font, top_line, 80, white);

    char word_display_buf[128];
    format_word_display(state.masked_word, state.word_len, word_display_buf);
    render_centered_text(renderer, font, word_display_buf, 300, white); 

    render_centered_text(renderer, font, state.status_msg, 160, white); 

    render_hangman(renderer, state.mistakes);
    
    pthread_mutex_unlock(&state.state_mutex);

    render_keyboard(renderer, font); 
    
    SDL_RenderPresent(renderer);
}



