#include <glue/interrupt.h>

#include <kernel/interrupt.h>

#include <arch/page_fault.h>
#include <arch/cpu.h>
#include <arch/pic.h>
#include <arch/mp.h>

struct interrupt_glue interrupt_glue_dispatcher =
{
    i386_interrupt_initialize,
    i386_interrupt_enable,
    i386_interrupt_disable,
    i386_interrupt_acknowledge,
    i386_interrupt_mask,
    i386_interrupt_unmask,
};

int i386_interrupt_initialize(void)
{
    mp_parse_tables();

    idt_initialize();
    pic_initialize();

    if (!interrupt_register(IRQ_PAGE_FAULT, INTERRUPT_CALLBACK,
                            page_fault_handler))
        return 0;

    return 1;
}

int i386_interrupt_enable(void)
{
    cpu_irq_enable();

    return 1;
}

int i386_interrupt_disable(void)
{
    cpu_irq_disable();

    return 1;
}

int i386_interrupt_acknowledge(int irq)
{
    if (irq >= PIC_START_IRQ && irq <= PIC_END_IRQ)
        pic_acnowledge(irq);

    return 1;
}

int i386_interrupt_mask(int irq)
{
    if (irq < PIC_START_IRQ || irq > PIC_END_IRQ)
        return 1;

    pic_mask(irq - PIC_START_IRQ);

    return 1;
}

int i386_interrupt_unmask(int irq)
{
    if (irq < PIC_START_IRQ || irq > PIC_END_IRQ)
        return 1;

    pic_unmask(irq - PIC_START_IRQ);

    return 1;
}
