#include "server.h"
#include "../common/utils.h"
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <ctype.h> 

Client *clients[100];
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void setup_level(Client *c) {
    int n = 0;
    switch (c->level) {
        case 1: n = 4; break;
        case 2: n = 3; break;
        case 3: n = 2; break;
        default: return;
    }
    strcpy(c->word, get_random_word(n));
    printf("[DEBUG] Player %d level %d word: %s\n",
       c->id, c->level, c->word);
    fflush(stdout);

    for (int i = 0; i < n; i++) {
        c->word[i] = toupper((unsigned char)c->word[i]);
    }
    c->word[n] = '\0';
    c->len = n;
    c->errors    = 0;
    memset(c->guessed, 0, sizeof(c->guessed)); 
    memset(c->mask, '_', n);
    c->mask[n] = '\0';
}
void send_msg(Client *c, char *msg) {
    send(c->fd, msg, strlen(msg), 0);
}
void check_timeout(Client *c) {
    time_t now = time(NULL);
    double x = difftime(now, c->start);

    if (x >= c->limit) {
        char buf[BUFFER_SIZE];
        snprintf(buf, sizeof(buf), "FINAL_WORD %s\n", c->word);
        send_msg(c, buf);
        snprintf(buf, sizeof(buf), "GAME_OVER LOSE\n");
        send_msg(c, buf);
        c->active = 0;
        //update_leaderboard(c->id, "LOST");
    }
}
void process_guess(Client *c, char ch) {
    ch = toupper(ch);
    char buf[BUFFER_SIZE]; 

    if (strchr(c->guessed, ch)) {
        snprintf(buf, sizeof(buf), "ALREADY_GUESSED %c\n", ch);
        send_msg(c, buf);
        return; 
    }
    size_t n = strlen(c->guessed);
    c->guessed[n] = ch;
    c->guessed[n + 1] = '\0';
    int hit = 0;
    int len = strlen(c->word);

    for (int i = 0; i < len; i++) {
        if (toupper((unsigned char)c->word[i]) == ch) {
            c->mask[i] = ch;
            hit = 1;
    }
}
    if (!hit) {
        c->errors++;
    }
    if (c->errors >= 7) {
        snprintf(buf, sizeof(buf), "UPDATE %s %d\n", c->word, c->errors);
        send_msg(c, buf);
        snprintf(buf, sizeof(buf), "FINAL_WORD %s\n", c->word);
        send_msg(c, buf);
        snprintf(buf, sizeof(buf), "GAME_OVER LOSE\n");
        send_msg(c, buf);
        c->active = 0; 
        //update_leaderboard(c->id, "LOST"); 
        return;
    }


    if (strcmp(c->mask, c->word) == 0) {
        c->level++;
        if (c->level > 3) {
            char win[BUFFER_SIZE]; 
            snprintf(win, sizeof(win), "WINNER Player_%d\n", c->id);
            pthread_mutex_lock(&mutex);
            
            for (int i = 0; i < count; i++) {
                
                if (clients[i] && clients[i]->active) {
                    send_msg(clients[i], win);
                }
            }
            pthread_mutex_unlock(&mutex);
            //update_leaderboard(c->id, "WON");
            return;
        }
        snprintf(buf, sizeof(buf), "LEVEL_COMPLETE %d\n", c->level - 1);
        send_msg(c, buf);
        setup_level(c);
        snprintf(buf, sizeof(buf), "LEVEL %d\n", c->level);
        send_msg(c, buf);
        snprintf(buf, sizeof(buf), "WORD %s\n", c->mask);
        send_msg(c, buf);
        snprintf(buf, sizeof(buf), "WORD_LEN %d\n", c->len);
        send_msg(c, buf);
        snprintf(buf, sizeof(buf), "GUESSED %s\n", c->guessed);
        send_msg(c, buf);
    } 
        
    else {
        snprintf(buf, sizeof(buf), "UPDATE %s %d\n", c->mask, c->errors);
        send_msg(c, buf);
        snprintf(buf, sizeof(buf), "GUESSED %s\n", c->guessed);
        send_msg(c, buf);
    }
}
void *client_handler(void *arg) {
    Client *c = (Client *)arg;
    char buf[BUFFER_SIZE];
    c->limit = 120;         
    c->start = time(NULL);  
    setup_level(c);
    char msg[BUFFER_SIZE];
    snprintf(msg, sizeof(msg), "GAME_START\nLEVEL %d\nWORD %s\nWORD_LEN %d\nTIMER %d\n", c->level, c->mask, c->len, c->limit);
    send_msg(c, msg);
    snprintf(msg, sizeof(msg), "GUESSED %s\n", c->guessed);
    send_msg(c, msg);

    while (c->active) {
        check_timeout(c);
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(c->fd, &fds);
        struct timeval tv;
        tv.tv_sec  = 0;
        tv.tv_usec = 100000;
        int x = select(c->fd + 1, &fds, NULL, NULL, &tv);
        if (x < 0) {
            break;
        }
        if (FD_ISSET(c->fd, &fds)) {
            int n = read(c->fd, buf, BUFFER_SIZE - 1);

            if (n <= 0) {
                c->active = 0;
                break;
            }
            buf[n] = '\0';

            if (strncmp(buf, "GUESS", 5) == 0) {
                char ch = buf[6];
                process_guess(c, ch);
            }
        }
    }
    close(c->fd);
    free(c);
    return NULL;
}
int main() {
    init_words(); 
    int fd = create_server_socket(PORT);
    printf("Server started on port %d\n", PORT);

    while (1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int sock = accept(fd, (struct sockaddr *)&addr, &len);
        
        if (sock < 0) {
            continue;
        }
        pthread_mutex_lock(&mutex);
        Client *c = malloc(sizeof(Client));
        c->fd = sock;
        c->id        = count++;
        c->level     = 1;
        c->active    = 1;
        memset(c->guessed, 0, sizeof(c->guessed)); 
        clients[c->id] = c;
        pthread_mutex_unlock(&mutex);
        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, (void *)c);
        pthread_detach(tid);
    }
    return 0;
}








