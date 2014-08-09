#ifndef THREAD_H
# define THREAD_H

# include <kernel/types.h>
# include <kernel/klist.h>
# include <kernel/process.h>

# include <arch/cpu.h>

# define THREAD_MAX_PER_PROCESS 10

struct thread
{
    size_t uid;
    size_t gid;

    uintptr_t kstack;

    struct thread_regs regs;

    struct klist list;
};

struct thread_glue
{
    int (*create)(struct process *, struct thread *, uintptr_t);
};

extern struct thread_glue _thread;

int thread_create(struct process *process, uintptr_t code);

#endif /* !THREAD_H */
