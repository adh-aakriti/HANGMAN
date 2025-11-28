#include "server.h"
#include "../common/utils.h"
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <ctype.h> 

Client *clients[100];
int client_count = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void setup_level(Client *cli) {
    int len = 0;
    int time_sec = 0;

    switch (cli->level) {
        case 1: len = 4; time_sec = 120; break;
        case 2: len = 3; time_sec = 60;  break;
        case 3: len = 2; time_sec = 15;  break;
        default: return;
    }

    strcpy(cli->current_word, get_random_word(len));

    cli->word_len = len;
    cli->time_limit  = time_sec;
    cli->start_time  = time(NULL);
    cli->mistakes    = 0;
    
    memset(cli->guessed_letters, 0, sizeof(cli->guessed_letters)); 

    memset(cli->masked_word, '_', len);
    cli->masked_word[len] = '\0';
}

void send_msg(Client *cli, char *msg) {
    send(cli->socket_fd, msg, strlen(msg), 0);
}

void check_timeout(Client *cli) {
    time_t now = time(NULL);
    double elapsed = difftime(now, cli->start_time);

    if (elapsed >= cli->time_limit) {
        char buf[BUFFER_SIZE];

        setup_level(cli);

        send_msg(cli, "TIME_UP\n");

        snprintf(buf, sizeof(buf), "NEW_WORD %s\n", cli->masked_word);
        send_msg(cli, buf);
        
        snprintf(buf, sizeof(buf), "WORD_LEN %d\n", cli->word_len);
        send_msg(cli, buf);

        snprintf(buf, sizeof(buf), "TIMER %d\n", cli->time_limit);
        send_msg(cli, buf);
        
        snprintf(buf, sizeof(buf), "GUESSED %s\n", cli->guessed_letters);
        send_msg(cli, buf);
    }
}

void process_guess(Client *cli, char letter) {
    letter = toupper(letter);
    char buf[BUFFER_SIZE]; 

    if (strchr(cli->guessed_letters, letter)) {
        snprintf(buf, sizeof(buf), "ALREADY_GUESSED %c\n", letter);
        send_msg(cli, buf);
        return; 
    }

    size_t len_guessed = strlen(cli->guessed_letters);
    cli->guessed_letters[len_guessed] = letter;
    cli->guessed_letters[len_guessed + 1] = '\0';
    
    int found = 0;
    int len = strlen(cli->current_word);

    for (int i = 0; i < len; i++) {
        if (cli->current_word[i] == letter) {
            cli->masked_word[i] = letter;
            found = 1;
        }
    }

    if (!found) {
        cli->mistakes++;
    }

    if (cli->mistakes >= 7) {
        snprintf(buf, sizeof(buf), "UPDATE %s %d\n", cli->current_word, cli->mistakes);
        send_msg(cli, buf);
        
        snprintf(buf, sizeof(buf), "FINAL_WORD %s\n", cli->current_word);
        send_msg(cli, buf);

        snprintf(buf, sizeof(buf), "GAME_OVER LOSE\n");
        send_msg(cli, buf);
        cli->active = 0; 
        return;
    }


    if (strcmp(cli->masked_word, cli->current_word) == 0) {

        cli->level++;

        if (cli->level > 3) {
            char winner_buf[BUFFER_SIZE]; 
            snprintf(winner_buf, sizeof(winner_buf), "WINNER Player_%d\n", cli->id);
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < client_count; i++) {
                if (clients[i] && clients[i]->active) {
                    send_msg(clients[i], winner_buf);
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            return;
        }

        snprintf(buf, sizeof(buf), "LEVEL_COMPLETE %d\n", cli->level - 1);
        send_msg(cli, buf);

        setup_level(cli);

        snprintf(buf, sizeof(buf), "LEVEL %d\n", cli->level);
        send_msg(cli, buf);

        snprintf(buf, sizeof(buf), "WORD %s\n", cli->masked_word);
        send_msg(cli, buf);
        
        snprintf(buf, sizeof(buf), "WORD_LEN %d\n", cli->word_len);
        send_msg(cli, buf);

        snprintf(buf, sizeof(buf), "TIMER %d\n", cli->time_limit);
        send_msg(cli, buf);
        
        snprintf(buf, sizeof(buf), "GUESSED %s\n", cli->guessed_letters);
        send_msg(cli, buf);

    } else {
        snprintf(buf, sizeof(buf), "UPDATE %s %d\n", cli->masked_word, cli->mistakes);
        send_msg(cli, buf);
        
        snprintf(buf, sizeof(buf), "GUESSED %s\n", cli->guessed_letters);
        send_msg(cli, buf);
    }
}

void *client_handler(void *arg) {
    Client *cli = (Client *)arg;
    char buffer[BUFFER_SIZE];

    setup_level(cli);

    char msg[BUFFER_SIZE];
    
    snprintf(msg, sizeof(msg),
             "GAME_START\nLEVEL %d\nWORD %s\nWORD_LEN %d\nTIMER %d\n", 
             cli->level, cli->masked_word, cli->word_len, cli->time_limit);

    send_msg(cli, msg);
    
    snprintf(msg, sizeof(msg), "GUESSED %s\n", cli->guessed_letters);
    send_msg(cli, msg);

    while (cli->active) {
        check_timeout(cli);
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(cli->socket_fd, &readfds);

        struct timeval tv;
        tv.tv_sec  = 0;
        tv.tv_usec = 100000;

        int activity = select(cli->socket_fd + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0) {
            break;
        }

        if (FD_ISSET(cli->socket_fd, &readfds)) {
            int valread = read(cli->socket_fd, buffer, BUFFER_SIZE - 1);
            if (valread <= 0) {
                cli->active = 0;
                break;
            }

            buffer[valread] = '\0';
            if (strncmp(buffer, "GUESS", 5) == 0) {
                char letter = buffer[6];
                process_guess(cli, letter);
            }
        }
    }

    close(cli->socket_fd);
    free(cli);
    return NULL;
}

int main() {
    init_words(); 

    int server_fd = create_server_socket(PORT);
    printf("Server started on port %d\n", PORT);

    while (1) {
        struct sockaddr_in address;
        socklen_t addrlen = sizeof(address);

        int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket < 0) {
            continue;
        }

        pthread_mutex_lock(&clients_mutex);

        Client *cli = malloc(sizeof(Client));
        cli->socket_fd = new_socket;
        cli->id        = client_count++;
        cli->level     = 1;
        cli->active    = 1;
        
        memset(cli->guessed_letters, 0, sizeof(cli->guessed_letters)); 

        clients[cli->id] = cli;

        pthread_mutex_unlock(&clients_mutex);

        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, (void *)cli);
        pthread_detach(tid);
    }

    return 0;
}
