#ifndef I386_PC_GLUE_EVENT_H
# define I386_PC_GLUE_EVENT_H

# include <arch/idt.h>

void i386_event_initialize(void);
void i386_event_enable(void);
void i386_event_disable(void);
void i386_event_acknowledge(int irq);
void i386_event_mask(int irq);
void i386_event_unmask(int irq);

#endif /* !I386_PC_GLUE_EVENT_H */
