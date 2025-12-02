#include "client.h" 
#include "../common/utils.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
GameState state;

static int connect_to_server(const char *ip) { //static so only this .c file can view this fucntion
    int sock = socket(AF_INET, SOCK_STREAM, 0); // creates TCP socket 
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (!ip || strcmp(ip, "127.0.0.1") == 0) { // checks the ip value
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    } 
        
    else {
        if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) { // converts ip into binary 
            perror("inet_pton");
            close(sock);
            return -1;
        }
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) { //connection attempt
        perror("connect");
        close(sock);
        return -1;
    }
    return sock;
}

int main(int argc, char const *argv[]){ //sets ip to stdrd 127 unless client gives 2 arguments 
    const char *ip = "127.0.0.1";

    if (argc >= 2) {
        ip = argv[1];
    }
    memset(&state, 0, sizeof(state));
    pthread_mutex_init(&state.mutex, NULL);
    state.fd = connect_to_server(ip);
    
    if (state.fd < 0) {
        fprintf(stderr, "could not connect to server\n");
        return 1;
    }

    state.running = 1;
    state.start = SDL_GetTicks();
    state.time = 0;
    state.win = 0;
    pthread_t net;
    
    if (pthread_create(&net, NULL, network_listen_thread, NULL) != 0) { //creates new thread 
        perror("pthread_create");
        close(state.fd);
        return 1;
    }
    SDL_Window *win = init_sdl(); //calls func to init a window
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //create renderer
    
    if (!ren) {
        die("renderer failed");
    }
    TTF_Font *font = TTF_OpenFont("../assets/FreeSans.ttf", 24); 
    
    if (!font) {
        fprintf(stderr, "could not open font: %s\n", TTF_GetError());
    }
    SDL_Event ev;
    Uint32 last = SDL_GetTicks();

    while (state.running) {

        while (SDL_PollEvent(&ev)) {
            
            if (ev.type == SDL_QUIT) {
                state.running = 0;    
            } 
                
            else if (ev.type == SDL_KEYDOWN) { //reads keyboard input and assigns variable to its code
                SDL_Keycode key = ev.key.keysym.sym;
        
                if (!state.over && key >= SDLK_a && key <= SDLK_z) { //won't process guess if not in right state or right key 
                    char c = (char)('a' + (key - SDLK_a));
                    send_guess(c);
                }
                    
                else if (state.over && key == SDLK_ESCAPE) {
                    state.running = 0;
                }
            }
        }
        Uint32 now = SDL_GetTicks(); //timer updating
        
        if (now - last > 1000) {
            last = now;
            pthread_mutex_lock(&state.mutex);
            
            if (!state.over && state.timer > 0) {
                state.timer--;
            }
            pthread_mutex_unlock(&state.mutex);
        }

        render_game(ren, font); //redraws game
        SDL_Delay(16); //delay to not hog CPU
    }
    //cleaning up:
    state.running = 0; 
    pthread_join(net, NULL);
    close(state.fd);
    
    if (font){
        TTF_CloseFont(font);
    }
    cleanup_sdl(win, ren);
    pthread_mutex_destroy(&state.mutex);
    return 0;
}




