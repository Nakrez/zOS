#ifndef EVENT_H
# define EVENT_H

# include <kernel/zos.h>

# include <glue/event.h>

# include <arch/cpu.h>

# define EVENT_NONE 0
# define EVENT_CALLBACK 1
# define EVENT_MESSAGE 2

# define EVENT_REGISTERED (1 << 0)
# define EVENT_FIRED (1 << 1)

struct event_glue
{
    int (*init)(void);
    int (*enable)(void);
    int (*disable)(void);
    int (*acnowledge)(int);
    int (*mask)(int);
    int (*unmask)(int);
};

struct event_entry
{
    int type;
    void (*callback)(struct irq_regs *);
};

extern struct event_glue event_glue_dispatcher;

/*
 * Initialize event mechanism
 */
void event_initialize(void);

/*
 * Enable event (interruptions)
 */
static inline void event_enable(void)
{
    glue_call(event, enable);
}

/*
 * Disable event (interruption)
 */
static inline void event_disable(void)
{
    glue_call(event, disable);
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
int event_register(int irq, int type, void (*callback)(struct irq_regs *));

void event_unregister(int irq);

/*
 * Mask an IRQ
 */
static inline void event_mask(int irq)
{
    glue_call(event, mask, irq);
}

/*
 * Unmask an IRQ
 */
static inline void event_unmask(int irq)
{
    glue_call(event, unmask, irq);
}

#endif /* !EVENT_H */
