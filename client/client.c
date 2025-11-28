#include "client.h"
#include "../common/utils.h"
#include <arpa/inet.h>
#include <unistd.h>

GameState state;

void connect_to_server(char* ip, int port) 
{
    struct sockaddr_in serv_addr;

    state.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (state.socket_fd < 0)
    {
        printf("Error creating socket!\n");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        printf("Invalid IP address.\n");
        exit(1)
    }

    if (connect(state.socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Could not connect to server.\n");
        exit(1)
    }
}

int main(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return -1;
    }

    // Init State
    state.running = 1;
    state.level = 1;
    state.mistakes = 0;
    state.timer_val = 120;
    strcpy(state.status_msg, "Connecting...");
    pthread_mutex_init(&state.state_mutex, NULL);

    // SDL Setup
    SDL_Window *win = init_sdl();
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // NOTE: Ensure a .ttf file exists in assets or root
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    if (!font) 
    {
        printf("Font not found, trying local file...\n");
        font = TTF_OpenFont("assets/font.ttf", 24);
        
        if(!font)
        {
            printf("Failed to load any font!! Put a .ttf in assets/\n");
            return 1;
        }
    }

    // Network Setup
    connect_to_server(argv[1], PORT);
    pthread_t net_thread;
    pthread_create(&net_thread, NULL, network_listen_thread, NULL);

    //Main Loop
    SDL_Event e;
    Uint32 last_timer_tick = SDL_GetTicks();

    while (state.running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (e.type == SDL_QUIT) 
            {
                state.running = 0;
            }
            else if (e.type == SDL_KEYDOWN) 
            {
                char key = e.key.keysym.sym;
                if (k >= 'a' && k <= 'z') 
                {
                    send_guess(k);
                }
            }
        }

        // Local Timer Countdown (Syncs with server roughly)
        Uint32 now = SDL_GetTicks();
        if (now - last_timer_tick >= 1000) 
        {
            pthread_mutex_lock(&state.state_mutex);
            
            if (state.timer_val > 0 && state.game_over) 
            {
                state.timer_val--;
            }
            pthread_mutex_unlock(&state.state_mutex);
            
            last_tick = now;
        }

        render_game(ren, font);
        
        SDL_Delay(16); // 60 FPS
    }
    close(state.socket_fd);
    cleanup_sdl(win, ren);
    return 0;
}


