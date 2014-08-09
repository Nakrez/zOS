#ifndef CPU_H
# define CPU_H

# include <kernel/scheduler.h>

/* No SMP for now, will be replaced by a variable when the time comes */
# define CPU_COUNT 1

struct cpu
{
    int id;

    struct scheduler scheduler;
};

void cpu_initialize(void);
void cpu_add_thread(struct thread *thread);

#endif /* !CPU_H */
