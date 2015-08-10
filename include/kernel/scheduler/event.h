#ifndef SCHEDULER_EVENT_H
# define SCHEDULER_EVENT_H

# include <kernel/klist.h>

# define SCHED_EV_NONE 0

/* Timer event */
# define SCHED_EV_TIMER 1

/* Interrupt event */
# define SCHED_EV_INTERRUPT 2

/* A new request has arrived on a channel */
# define SCHED_EV_REQ 3

/* A new response has arrived */
# define SCHED_EV_RESP 4

/* A process exited */
# define SCHED_EV_PEXIT 5

/* A process exited and notify parent */
# define SCHED_EV_PEXIT_PARENT 6

# define SCHED_EV_SIZE SCHED_EV_PEXIT_PARENT

struct thread;

struct scheduler_event {
    int event;
    int data;
};

void scheduler_event_initialize(void);
void scheduler_event_notify(int event, int data);
void scheduler_event_wait(int event, struct thread *thread);

#endif /* !SCHEDULER_EVENT_H */
