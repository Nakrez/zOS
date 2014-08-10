#ifndef SCHEDULER_H
# define SCHEDULER_H

# include <kernel/thread.h>
# include <kernel/timer.h>
# include <kernel/klist.h>

# include <arch/cpu.h>
# include <arch/spinlock.h>

# define SCHEDULER_TIME 25 / TIMER_GRANULARITY

struct scheduler
{
    /* Time left for the running task */
    size_t time;

    struct thread *running;

    /* Idle thread to elect when nothing runs */
    struct thread *idle;

    /* Number of thread in the scheduler list */
    size_t thread_num;

    spinlock_t sched_lock;

    struct klist threads;
};

struct scheduler_glue
{
    void (*sswitch)(struct irq_regs *r, struct thread *new,
                    struct thread *old);
};

extern struct scheduler_glue _scheduler;

/*
 * Initialize a scheduler
 */
void scheduler_initialize(struct scheduler *sched);

/*
 * Add an idle thread to a scheduler
 */
void scheduler_add_idle(struct scheduler *sched, struct thread *idle);
/*
 * Add a thread to a specific scheduler
 */
void scheduler_add_thread(struct scheduler *sched, struct thread *thread);

/*
 * Start a scheduler
 */
void scheduler_start(struct scheduler *sched);

/*
 * This is the core of the scheduling process
 * The function is called by the timer on regular basis to update scheduler
 * state
 */
void scheduler_update(struct irq_regs *regs);

/*
 * Elect a new thread
 */
struct thread *scheduler_elect(struct scheduler *sched);

/*
 * Switch to new_thread
 */
void scheduler_switch(struct scheduler *sched, struct thread *new_thread,
                      struct irq_regs *regs);

#endif /* !SCHEDULER_H */
