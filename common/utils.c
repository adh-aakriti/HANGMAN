#include "utils.h"

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void trim_newline(char *str) {
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}
