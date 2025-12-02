#include "utils.h"

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
void trim_newline(char *str) {
    
    if (!str) return;

    for (char *p = str; *p; ++p) {
        
        if (*p == '\n' || *p == '\r') {
            *p = '\0';
            break;
        }
    }
}

