#ifndef TIMER_H
# define TIMER_H

# include <kernel/types.h>
# include <kernel/klist.h>

# include <arch/cpu.h>

/* 1 ms */
# define TIMER_GRANULARITY 1

# define TIMER_NONE 0
# define TIMER_CALLBACK (1 << 1)
# define TIMER_MESSAGE (1 << 2)
# define TIMER_ONE_SHOT (1 << 3)
# define TIMER_INFINITE (1 << 4)

struct timer_glue
{
    void (*init)(void);
};

struct timer_entry
{
    int type;
    int data;
    size_t timer;
    size_t count;
    void (*callback)(int);

    struct klist list;
};

extern struct timer_glue __timer;

void timer_initialize(void);
void timer_handler(struct irq_regs *regs);
int timer_register(int cpu_id, int type, int data, size_t time,
                   void (*callback)(int));

#endif /* !TIMER_H */
