#ifndef CPU_H
# define CPU_H

# include <kernel/scheduler.h>

# include <glue/cpu.h>

/* No SMP for now, will be replaced by a variable when the time comes */
# define CPU_COUNT 1

struct cpu
{
    int id;

    struct scheduler scheduler;

    struct cpu_glue_data arch;
};

struct cpu_glue
{
    void (*init)(struct cpu *);
};

extern struct cpu_glue cpu_glue_dispatcher;

void cpu_initialize(void);
void cpu_add_thread(struct thread *thread);
void cpu_start(void);

/*
 * Get cpu structure from an id
 */
struct cpu *cpu_get(int id);

/* FIXME: Smp get current cpu id */
static inline int cpu_id_get(void)
{
    return 0;
}

#endif /* !CPU_H */
