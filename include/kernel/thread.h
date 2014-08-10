#ifndef THREAD_H
# define THREAD_H

# include <kernel/types.h>
# include <kernel/klist.h>
# include <kernel/process.h>

# include <arch/cpu.h>

# define THREAD_MAX_PER_PROCESS 10

# define THREAD_STATE_RUNNING 1
# define THREAD_STATE_BLOCKED 2

struct thread
{
    int state;

    struct process *parent;

    size_t uid;
    size_t gid;

    int cpu;

    uintptr_t kstack;

    struct thread_regs regs;

    /* List of thread in process */
    struct klist list;

    /* List for the scheduler */
    struct klist sched;
};

struct thread_glue
{
    int (*create)(struct process *, struct thread *, uintptr_t);
    struct thread *(*current)(void);
};

extern struct thread_glue _thread;

/*
 * Create a new thread inside a process
 */
int thread_create(struct process *process, uintptr_t code);

/*
 * Get the running thread
 */
static inline struct thread *thread_current(void)
{
    return _thread.current();
}

#endif /* !THREAD_H */
