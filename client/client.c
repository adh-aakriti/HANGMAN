#include "client.h"
#include "../common/utils.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

GameState state;

static int connect_to_server(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);      
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

int main(int argc, char const *argv[]){
    const char *server_ip = "127.0.0.1";

    if (argc >= 2) {
        server_ip = argv[1];
    }

    memset(&state, 0, sizeof(state));
    pthread_mutex_init(&state.state_mutex, NULL);

    state.socket_fd = connect_to_server();
    if (state.socket_fd < 0) {
        fprintf(stderr, "could not connect to server\n");
        return 1;
    }

    state.running = 1;

    pthread_t net_thread;
    if (pthread_create(&net_thread, NULL, network_listen_thread, NULL) != 0) {
        perror("pthread_create");
        close(state.socket_fd);
        return 1;
    }

    SDL_Window *win = init_sdl();
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) die("renderer failed");

    TTF_Font *font = TTF_OpenFont("assets/FreeSans.ttf", 24);
    if (!font) {

        fprintf(stderr, "could not open font: %s\n", TTF_GetError());
    }

    SDL_Event ev;
    Uint32 last_timer_tick = SDL_GetTicks();

    while (state.running && !state.game_over) {

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                state.running = 0;
            } else if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode key = ev.key.keysym.sym;

                if (key >= SDLK_a && key <= SDLK_z) {
                    char letter = (char)('A' + (key - SDLK_a)); 
                    send_guess(letter);
                }
            }
                }
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - last_timer_tick > 1000) {
            last_timer_tick = now;
            
            pthread_mutex_lock(&state.state_mutex);
            if (state.timer_val > 0) {
                state.timer_val--;
            }
            pthread_mutex_unlock(&state.state_mutex);
            
        }

        render_game(ren, font);
        SDL_Delay(16);
    }

    state.running = 0;
    pthread_join(net_thread, NULL);

    close(state.socket_fd);

    if (font) TTF_CloseFont(font);
    cleanup_sdl(win, ren);
    pthread_mutex_destroy(&state.state_mutex);

    return 0;
}








