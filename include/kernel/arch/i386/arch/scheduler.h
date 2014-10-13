#ifndef ARCH_I386_SCHEDULER_H
# define ARCH_I386_SCHEDULER_H

# include <kernel/proc/thread.h>

# include <arch/cpu.h>

void i386_switch(struct irq_regs *regs, struct thread *new,
                 struct thread *old);

#endif /* !ARCH_I386_SCHEDULER_H */
