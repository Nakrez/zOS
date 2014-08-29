#ifndef SCHEDULER_EVENT_H
# define SCHEDULER_EVENT_H

# include <kernel/thread.h>
# include <kernel/klist.h>

# define SCHED_EV_NONE 0

/* Timer event */
# define SCHED_EV_TIMER 1

/* Interrupt event */
# define SCHED_EV_INTERRUPT 2

/* A new request has arrived on a channel */
# define SCHED_EV_REQ 3

struct scheduler_event {
    int event;
    int data;
};

struct scheduler_wait {
    struct thread *thread;

    struct klist list;
};

void scheduler_event_notify(int event, int data);
void scheduler_wait_event(struct thread *thread);

#endif /* !SCHEDULER_EVENT_H */
