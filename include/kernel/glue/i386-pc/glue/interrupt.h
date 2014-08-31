#ifndef I386_PC_GLUE_INTERRUPT_H
# define I386_PC_GLUE_INTERRUPT_H

# include <arch/idt.h>

int i386_interrupt_initialize(void);
int i386_interrupt_enable(void);
int i386_interrupt_disable(void);
int i386_interrupt_acknowledge(int irq);
int i386_interrupt_mask(int irq);
int i386_interrupt_unmask(int irq);

#endif /* !I386_PC_GLUE_INTERRUPT_H */
