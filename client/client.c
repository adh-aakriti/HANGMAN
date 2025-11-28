#include "client.h"
#include "../common/utils.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

// Global game state object (declared in client.h)
GameState state;

static int connect_to_server(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    // Set a timeout for the connection attempt
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    // NOTE: This setting is often better done using setsockopt(SO_RCVTIMEO/SO_SNDTIMEO)
    // but we'll stick to the current structure.

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);      
    // Try to connect to the loopback address (local machine)
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

int main(void) {
    memset(&state, 0, sizeof(state));
    pthread_mutex_init(&state.state_mutex, NULL);

    state.socket_fd = connect_to_server();
    if (state.socket_fd < 0) {
        fprintf(stderr, "Failed to connect to server. Ensure server is running.\n");
        return 1;
    }

    state.running = 1;
    // Removed unused variable 'server_ip' to fix a warning

    pthread_t net_thread;
    if (pthread_create(&net_thread, NULL, network_listen_thread, NULL) != 0) {
        perror("pthread_create");
        close(state.socket_fd);
        return 1;
    }

    // --- SDL Initialization ---
    SDL_Window *win = init_sdl();
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) die("renderer failed");

    TTF_Font *font = TTF_OpenFont("assets/FreeSans.ttf", 24);
    if (!font) {
        fprintf(stderr, "Could not open font: %s\n", TTF_GetError());
    }

    // --- Main Game Loop ---
    SDL_Event ev;
    // FIX: last_timer_tick is now correctly used and initialized
    Uint32 last_timer_tick = SDL_GetTicks(); 

    while (state.running && !state.game_over) {

        // 1. Handle Input Events
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                state.running = 0;
            } else if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode key = ev.key.keysym.sym;

                if (key >= SDLK_a && key <= SDLK_z) {
                    // FIX: Send the uppercase version of the letter
                    char letter = (char)('A' + (key - SDLK_a));
                    send_guess(letter);
                }
            }
        }

        // 2. Handle Timer Logic (Runs about once per second)
        Uint32 now = SDL_GetTicks();
        if (now - last_timer_tick > 1000) {
            
            // FIX: The core timer logic was missing.
            pthread_mutex_lock(&state.state_mutex);
            if (state.timer_val > 0) {
                state.timer_val--;
            }
            pthread_mutex_unlock(&state.state_mutex);
            
            last_timer_tick = now;
        }

        // 3. Render Game State
        render_game(ren, font);
        
        // 4. Cap Frame Rate (16ms = ~60 FPS)
        SDL_Delay(16);
    } 
    // --- End Main Game Loop ---

    // --- Cleanup ---
    // The previous errors were caused by the compiler misinterpreting the closing brace position 
    // of the while loop, leading to all cleanup functions being seen as variable declarations.
    
    // Signal the network thread to stop
    state.running = 0; 
    
    // Wait for the network thread to finish gracefully
    pthread_join(net_thread, NULL); 

    // Close socket
    if (state.socket_fd >= 0) {
        close(state.socket_fd);
    }

    // Close font and SDL resources
    if (font) TTF_CloseFont(font);
    cleanup_sdl(win, ren);

    return 0;
}
