#include "utils.h" //includes utils.c so it knows the functions

void die(const char *msg) { //prints an the error if there is one
    perror(msg);
    exit(EXIT_FAILURE);
}
void trim_newline(char *str) {
    
    if (!str) return;

    for (char *p = str; *p; ++p) { // declares pointer p starting at str and ending when *p = \0
        
        if (*p == '\n' || *p == '\r') { 
            *p = '\0';
            break;
        }
    }
}


