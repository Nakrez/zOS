#include <glue/event.h>

#include <kernel/event.h>

#include <arch/cpu.h>
#include <arch/pic.h>

struct event_glue __event =
{
    i386_event_initialize,
    i386_event_enable,
    i386_event_disable,
    i386_event_acknowledge,
    i386_event_mask,
    i386_event_unmask,
};

void i386_event_initialize(void)
{
    idt_initialize();
    pic_initialize();
}

void i386_event_enable(void)
{
    cpu_irq_enable();
}

void i386_event_disable(void)
{
    cpu_irq_disable();
}

void i386_event_acknowledge(int irq)
{
    if (irq >= PIC_START_IRQ && irq <= PIC_END_IRQ)
        pic_acnowledge(irq);
}

void i386_event_mask(int irq)
{
    if (irq < PIC_START_IRQ || irq > PIC_END_IRQ)
        return;

    pic_mask(irq - PIC_START_IRQ);
}

void i386_event_unmask(int irq)
{
    if (irq < PIC_START_IRQ || irq > PIC_END_IRQ)
        return;

    pic_unmask(irq - PIC_START_IRQ);
}
