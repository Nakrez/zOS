#include <kernel/panic.h>

#include <arch/scheduler.h>

void i386_switch(struct irq_regs *regs, struct thread *new,
                 struct thread *old)
{
    (void) regs;
    (void) new;
    (void) old;

    kernel_panic("i386_switch: Implementation needed");
}
