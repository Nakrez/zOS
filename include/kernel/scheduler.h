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

    size_t thread_num;

    spinlock_t sched_lock;

    struct klist threads;
};

/*
 * Initialize a scheduler
 */
void scheduler_initialize(struct scheduler *sched);

/*
 * Add a thread to a specific scheduler
 */
void scheduler_add_thread(struct scheduler *sched, struct thread *thread);

/*
 * This is the core of the scheduling process
 * The function is called by the timer on regular basis to update scheduler
 * state
 */
void scheduler_update(struct irq_regs *regs);

#endif /* !SCHEDULER_H */
