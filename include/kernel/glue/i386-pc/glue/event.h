#ifndef I386_PC_GLUE_EVENT_H
# define I386_PC_GLUE_EVENT_H

# include <arch/idt.h>

int i386_event_initialize(void);
int i386_event_enable(void);
int i386_event_disable(void);
int i386_event_acknowledge(int irq);
int i386_event_mask(int irq);
int i386_event_unmask(int irq);

#endif /* !I386_PC_GLUE_EVENT_H */
