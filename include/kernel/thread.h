#ifndef THREAD_H
# define THREAD_H

# include <kernel/klist.h>
# include <arch/cpu.h>

# define THREAD_MAX_PER_PROCESS 10

struct thread
{
    size_t uid;
    size_t gid;

    struct thread_regs regs;

    struct klist list;
};

#endif /* !THREAD_H */
