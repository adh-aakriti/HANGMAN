#ifndef TIMER_H
#define TIMER_H

#include <time.h>

typedef struct {
    time_t start;
    int dur;
} GameTimer;

void timer_start(GameTimer *t, int dur);
int timer_is_expired(GameTimer *t);
int timer_get_remaining(GameTimer *t);

#endif
