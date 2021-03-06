#ifndef __TIMER_H__
#define __TIMER_H__

#define MAX_TIMER  (16)

struct FIFO;

typedef struct TIMER {
  struct TIMER* next_timer;
  unsigned int timeout;
  char flags, flags2;
  struct FIFO* fifo;
  int data;
} TIMER;

typedef struct {
  unsigned int count, next_time;
  TIMER* t0;
  TIMER timers0[MAX_TIMER];
} TIMERCTL;

extern TIMERCTL timerctl;

void init_pit(void);
TIMER* timer_alloc(void);
void timer_free(TIMER* timer);
void timer_init(TIMER* timer, struct FIFO* fifo, int data);
void timer_settime(TIMER* timer, unsigned int timeout);
int timer_cancel(TIMER* timer);
void timer_cancelall(struct FIFO* fifo);

#endif
