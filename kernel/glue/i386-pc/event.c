#include <glue/event.h>

#include <kernel/event.h>

#include <arch/page_fault.h>
#include <arch/cpu.h>
#include <arch/pic.h>

struct event_glue event_glue_dispatcher =
{
    i386_event_initialize,
    i386_event_enable,
    i386_event_disable,
    i386_event_acknowledge,
    i386_event_mask,
    i386_event_unmask,
};

int i386_event_initialize(void)
{
    idt_initialize();
    pic_initialize();

    if (!event_register(IRQ_PAGE_FAULT, EVENT_CALLBACK, page_fault_handler))
        return 0;

    return 1;
}

int i386_event_enable(void)
{
    cpu_irq_enable();

    return 1;
}

int i386_event_disable(void)
{
    cpu_irq_disable();

    return 1;
}

int i386_event_acknowledge(int irq)
{
    if (irq >= PIC_START_IRQ && irq <= PIC_END_IRQ)
        pic_acnowledge(irq);

    return 1;
}

int i386_event_mask(int irq)
{
    if (irq < PIC_START_IRQ || irq > PIC_END_IRQ)
        return 1;

    pic_mask(irq - PIC_START_IRQ);

    return 1;
}

int i386_event_unmask(int irq)
{
    if (irq < PIC_START_IRQ || irq > PIC_END_IRQ)
        return 1;

    pic_unmask(irq - PIC_START_IRQ);

    return 1;
}
