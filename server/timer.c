#include "timer.h"
#include <stdio.h>

void timer_start(GameTimer *t, int duration_seconds) {
    t->start_time = time(NULL);
    t->duration = duration_seconds;
}

int timer_is_expired(GameTimer *t) {
    time_t now = time(NULL);
    double elapsed = difftime(now, t->start_time);
    return elapsed >= t->duration;
}

int timer_get_remaining(GameTimer *t) {
    time_t now = time(NULL);
    double elapsed = difftime(now, t->start_time);
    int remaining = t->duration - (int)elapsed;
    return (remaining < 0) ? 0 : remaining;
}
