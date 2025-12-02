#include "timer.h"
#include <stdio.h>

void timer_start(GameTimer *t, int dur) {
    t->start = time(NULL);
    t->dur = dur;
}
int timer_is_expired(GameTimer *t) {
    time_t now = time(NULL);
    double x = difftime(now, t->start);
    return x >= t->dur;
}
int timer_get_remaining(GameTimer *t) {
    time_t now = time(NULL);
    double x = difftime(now, t->start);
    int left = t->dur - (int)x;
    return (left < 0) ? 0 : left;
}
