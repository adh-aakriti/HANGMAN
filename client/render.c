#include "client.h"
#include "../common/utils.h"
#include <stdio.h>
#include <string.h> // Include for strchr

// MODIFIED: Renamed from draw_text and added SDL_Color parameter
static void render_text(SDL_Renderer *ren, TTF_Font *font,
                      const char *txt, int x, int y, SDL_Color color) {
    if (!font || !txt) return;

    // Use the passed color
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

// NEW FUNCTION: Renders the QWERTY keyboard overlay
static void render_keyboard(SDL_Renderer *ren, TTF_Font *font) {
    int start_x = 50;
    int start_y = 500;
    int key_w = 40;
    int key_h = 40;
    int spacing = 5;
    
    // QWERTY layout representation (Uppercase)
    const char *rows[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    int row_count = sizeof(rows) / sizeof(rows[0]);

    // Define Colors
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color correct_green = {0, 150, 0, 255};
    SDL_Color incorrect_red = {150, 0, 0, 255};
    SDL_Color unguessed_gray = {100, 100, 100, 255};

    pthread_mutex_lock(&state.state_mutex);

    for (int r = 0; r < row_count; r++) {
        // Adjust starting X position to roughly center the rows
        int row_offset_x = (r == 0) ? 0 : (r == 1) ? (key_w + spacing) * 0.5 : (key_w + spacing) * 1.5; 
        
        for (int i = 0; rows[r][i] != '\0'; i++) {
            char letter = rows[r][i];
            
            SDL_Color key_color = unguessed_gray; 
            
            // Check if letter has been guessed
            if (strchr(state.guessed_letters, letter)) {
                
                // Correctly guessed letters are in the masked word
                if (strchr(state.masked_word, letter)) {
                    key_color = correct_green;
                } else {
                    // Incorrectly guessed letters are not in the masked word
                    key_color = incorrect_red;
                }
            }


            // Draw Key background
            SDL_Rect rect = {
                start_x + row_offset_x + i * (key_w + spacing), 
                start_y + r * (key_h + spacing), 
                key_w, 
                key_h
            };
            SDL_SetRenderDrawColor(ren, key_color.r, key_color.g, key_color.b, key_color.a);
            SDL_RenderFillRect(ren, &rect);
            
            // Draw Letter on Key (Always white text)
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

    char top_line[128];
    snprintf(top_line, sizeof(top_line),
             "Level: %d   Time: %d   Mistakes: %d",
             state.level, state.timer_val, state.mistakes);
    
    // Use the new render_text function
    render_text(renderer, font, top_line, 20, 20, white);

    render_text(renderer, font, state.masked_word, 20, 80, white);

    render_text(renderer, font, state.status_msg, 20, 140, white);

    pthread_mutex_unlock(&state.state_mutex);

    // CALL THE NEW KEYBOARD FUNCTION
    render_keyboard(renderer, font); 
    
    SDL_RenderPresent(renderer);
}
