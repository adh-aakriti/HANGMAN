#ifndef TIMER_H
#define TIMER_H

#include <time.h>

typedef struct {
    time_t start_time;
    int duration;
} GameTimer;

void timer_start(GameTimer *t, int duration_seconds);
int timer_is_expired(GameTimer *t);
int timer_get_remaining(GameTimer *t);

#endif
