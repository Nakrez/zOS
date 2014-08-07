#ifndef EVENT_H
# define EVENT_H

# include <arch/cpu.h>

struct event_glue
{
    void (*init)(void);
    void (*enable)(void);
    void (*disable)(void);
    void (*acnowledge)(int);
};

extern struct event_glue __event;

/*
 * Initialize event mechanism
 */
void event_initialize(void);

/*
 * Enable event (interruptions)
 */
static inline void event_enable(void)
{
    __event.enable();
}

/*
 * Disable event (interruption)
 */
static inline void event_disable(void)
{
    __event.disable();
}

/*
 * Dispatch irq to the destination (handler call or message)
 */
void event_dispatch(struct irq_regs *regs);

void event_acnowledge(int irq);

#endif /* !EVENT_H */
