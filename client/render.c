// client/render.c
#include "client.h"
#include "../common/utils.h"
#include <stdio.h>
#include <string.h>

// Global state variable (declared in client.h)
extern GameState state;

// NOTE: This function MUST be non-static (no 'static' keyword) because the prototype is in client.h
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

// Renders text centered on the screen (assuming 800 width)
static void render_centered_text(SDL_Renderer *ren, TTF_Font *font,
                                 const char *txt, int y, SDL_Color color) {
    int w, h;
    TTF_SizeText(font, txt, &w, &h);
    render_text(ren, font, txt, (800 - w) / 2, y, color);
}

// Function to space out the letters for clarity
static char* format_word_display(const char *masked_word, int len, char *buffer) {
    buffer[0] = '\0';
    for (int i = 0; i < len; i++) {
        // Append the letter/blank followed by a space
        char temp[3] = {masked_word[i], ' ', '\0'};
        strcat(buffer, temp);
    }
    return buffer;
}

// Renders the Hangman Figure based on mistake count (Max 7 mistakes for body parts)
static void render_hangman(SDL_Renderer *ren, int mistakes) {
    // Hangman gallows positioning (Left side of the screen)
    int gallows_x = 100;
    int gallows_y = 400; // Base line
    int post_height = 200;
    int head_radius = 15;
    int body_length = 40;
    int arm_length = 20;
    int leg_length = 25;
    
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255); // White color

    // --- Gallows Structure (Always drawn) ---

    // 1. Base / Platform
    SDL_RenderDrawLine(ren, gallows_x - 50, gallows_y, gallows_x + 50, gallows_y); 
    
    // 2. Vertical Post
    SDL_RenderDrawLine(ren, gallows_x, gallows_y, gallows_x, gallows_y - post_height); 
    
    // 3. Top Beam
    SDL_RenderDrawLine(ren, gallows_x, gallows_y - post_height, gallows_x + 70, gallows_y - post_height); 
    
    // 4. Rope / Noose
    int rope_x = gallows_x + 70;
    int rope_top_y = gallows_y - post_height;
    int head_center_y = rope_top_y + head_radius * 2;
    SDL_RenderDrawLine(ren, rope_x, rope_top_y, rope_x, head_center_y - head_radius);

    // --- Body Parts (7 mistakes) ---
    // Order: Head (M1), Body (M2), Arm1 (M3), Arm2 (M4), Leg1 (M5), Leg2 (M6), Dead Face (M7)

    // MISTAKE 1: Head 
    if (mistakes >= 1) {
        // Draw the Head (Simplified square approximation)
        SDL_Rect head = {rope_x - head_radius, head_center_y - head_radius, head_radius * 2, head_radius * 2};
        SDL_RenderDrawRect(ren, &head);
    }
    
    // MISTAKE 2: Body
    int body_top_y = head_center_y + head_radius;
    int body_bottom_y = body_top_y + body_length;
    if (mistakes >= 2) {
        SDL_RenderDrawLine(ren, rope_x, body_top_y, rope_x, body_bottom_y);
    }
    
    // MISTAKE 3: Left Arm
    int shoulder_y = body_top_y + 5;
    if (mistakes >= 3) {
        SDL_RenderDrawLine(ren, rope_x, shoulder_y, rope_x - arm_length, shoulder_y + arm_length);
    }
    
    // MISTAKE 4: Right Arm
    if (mistakes >= 4) {
        SDL_RenderDrawLine(ren, rope_x, shoulder_y, rope_x + arm_length, shoulder_y + arm_length);
    }
    
    // MISTAKE 5: Left Leg
    if (mistakes >= 5) {
        SDL_RenderDrawLine(ren, rope_x, body_bottom_y, rope_x - leg_length, body_bottom_y + leg_length);
    }
    
    // MISTAKE 6: Right Leg
    if (mistakes >= 6) {
        SDL_RenderDrawLine(ren, rope_x, body_bottom_y, rope_x + leg_length, body_bottom_y + leg_length);
    }
    
    // MISTAKE 7 (Final Mistake): Dead Face (X eyes)
    if (mistakes >= 7) {
        int eye_offset = head_radius / 3;
        // Left Eye (X)
        SDL_RenderDrawLine(ren, rope_x - eye_offset, head_center_y - eye_offset, rope_x - head_radius + eye_offset, head_center_y + eye_offset);
        SDL_RenderDrawLine(ren, rope_x - eye_offset, head_center_y + eye_offset, rope_x - head_radius + eye_offset, head_center_y - eye_offset);
        // Right Eye (X)
        SDL_RenderDrawLine(ren, rope_x + head_radius - eye_offset, head_center_y - eye_offset, rope_x + eye_offset, head_center_y + eye_offset);
        SDL_RenderDrawLine(ren, rope_x + head_radius - eye_offset, head_center_y + eye_offset, rope_x + eye_offset, head_center_y - eye_offset);
    }
}

// Renders the QWERTY keyboard overlay
static void render_keyboard(SDL_Renderer *ren, TTF_Font *font) {
    // Keyboard positioned low-center
    int start_x = 100; 
    int start_y = 450;
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
        // Adjust starting X position to roughly center the rows below the previous one
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

    // 1. Top Line (Mistakes show /7)
    char top_line[128];
    snprintf(top_line, sizeof(top_line),
             "Level: %d   Time: %d   Mistakes: %d/7", 
             state.level, state.timer_val, state.mistakes);
    render_text(renderer, font, top_line, 20, 20, white);

    // 2. Word Length Message (New Feature)
    char len_msg[64];
    snprintf(len_msg, sizeof(len_msg), "The word has %d letters", state.word_len);
    render_centered_text(renderer, font, len_msg, 70, white); // Positioned high

    // 3. Word Display (Centered and with spacing)
    char word_display_buf[128];
    // This is the line that shows the blank spaces/guessed letters
    format_word_display(state.masked_word, state.word_len, word_display_buf);
    render_centered_text(renderer, font, word_display_buf, 100, white); // Positioned slightly below word length

    // 4. Status Message (Centered)
    render_centered_text(renderer, font, state.status_msg, 160, white); 

    // 5. Hangman Figure (uses mistakes count)
    render_hangman(renderer, state.mistakes);
    
    pthread_mutex_unlock(&state.state_mutex);

    // 6. Keyboard (calls its own lock internally)
    render_keyboard(renderer, font); 
    
    SDL_RenderPresent(renderer);
}

