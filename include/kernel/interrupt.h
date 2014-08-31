#ifndef INTERRUPT_H
# define INTERRUPT_H

# include <kernel/zos.h>

# include <glue/interrupt.h>

# include <arch/cpu.h>

# define INTERRUPT_NONE 0
# define INTERRUPT_CALLBACK 1

# define INTERRUPT_REGISTERED (1 << 0)
# define INTERRUPT_FIRED (1 << 1)

struct interrupt_glue
{
    int (*init)(void);
    int (*enable)(void);
    int (*disable)(void);
    int (*acnowledge)(int);
    int (*mask)(int);
    int (*unmask)(int);
};

struct interrupt_entry
{
    int type;
    void (*callback)(struct irq_regs *);
};

extern struct interrupt_glue interrupt_glue_dispatcher;

/*
 * Initialize interrupt mechanism
 */
void interrupt_initialize(void);

/*
 * Enable interruptions
 */
static inline void interrupt_enable(void)
{
    glue_call(interrupt, enable);
}

/*
 * Disable interrupt (interruption)
 */
static inline void interrupt_disable(void)
{
    glue_call(interrupt, disable);
}

/*
 * Dispatch irq to the destination (handler call or message)
 */
void interrupt_dispatch(struct irq_regs *regs);

/*
 * Acnowledge the irq
 */
void interrupt_acnowledge(int irq);

/*
 * Register a new callback / message for the irq
 */
int interrupt_register(int irq, int type, void (*callback)(struct irq_regs *));

void interrupt_unregister(int irq);

/*
 * Mask an IRQ
 */
static inline void interrupt_mask(int irq)
{
    glue_call(interrupt, mask, irq);
}

/*
 * Unmask an IRQ
 */
static inline void interrupt_unmask(int irq)
{
    glue_call(interrupt, unmask, irq);
}

#endif /* !INTERRUPT_H */
