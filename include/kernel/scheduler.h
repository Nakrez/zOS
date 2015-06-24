#ifndef SCHEDULER_H
# define SCHEDULER_H

# include <kernel/klist.h>
# include <kernel/time.h>

# include <kernel/proc/thread.h>

# include <arch/cpu.h>
# include <arch/spinlock.h>

/**
 *  \brief  The value (in tick) where the scheduler will be ran
 */
# define SCHEDULER_TIME 10

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
    void (*sswitch)(struct irq_regs *, struct thread *, struct thread *old,
                    spinlock_t *);
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
void scheduler_update(struct irq_regs *regs, int force);

void scheduler_remove_thread(struct thread *t, struct scheduler *sched);

#endif /* !SCHEDULER_H */
