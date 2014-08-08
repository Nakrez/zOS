#ifndef EVENT_H
# define EVENT_H

# include <arch/cpu.h>

# define EVENT_NONE 0
# define EVENT_CALLBACK 1
# define EVENT_MESSAGE 2

struct event_glue
{
    void (*init)(void);
    void (*enable)(void);
    void (*disable)(void);
    void (*acnowledge)(int);
    void (*mask)(int);
    void (*unmask)(int);
};

struct event_entry
{
    int type;
    void (*callback)(int, int);
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

/*
 * Acnowledge the irq
 */
void event_acnowledge(int irq);

/*
 * Register a new callback / message for the irq
 */
int event_register(int irq, int type, void (*callback)(int, int));

/*
 * Mask an IRQ
 */
static inline void event_mask(int irq)
{
    __event.mask(irq);
}

/*
 * Unmask an IRQ
 */
static inline void event_unmask(int irq)
{
    __event.unmask(irq);
}

#endif /* !EVENT_H */
