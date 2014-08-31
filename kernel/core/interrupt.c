#include <string.h>

#include <kernel/interrupt.h>
#include <kernel/console.h>
#include <kernel/panic.h>

#include <glue/interrupt.h>

static struct interrupt_entry interrupt_entries[MAX_IRQ_NUMBER];

void interrupt_initialize(void)
{
    memset(interrupt_entries, 0, sizeof (interrupt_entries));

    glue_call(interrupt, init);
}

void interrupt_dispatch(struct irq_regs *regs)
{
    /*
     * FIXME: we use reg->irq_num and reg->irq_data that must be here
     * for every architecture ....
     */
    interrupt_acnowledge(regs->irq_num);

    if (regs->irq_num >= MAX_IRQ_NUMBER)
        kernel_panic("Invalid IRQ number");

    if (interrupt_entries[regs->irq_num].type == INTERRUPT_CALLBACK)
        interrupt_entries[regs->irq_num].callback(regs);
    else
        console_message(T_INF, "Unhandled IRQ %i fired with data = 0x%x",
                        regs->irq_num, regs->irq_data);

}

void interrupt_acnowledge(int irq)
{
    glue_call(interrupt, acnowledge, irq);
}

int interrupt_register(int irq, int type, void (*callback)(struct irq_regs *))
{
    if (type == INTERRUPT_NONE)
        return 0;

    if (irq < 0 || irq > MAX_IRQ_NUMBER)
        return 0;

    if (interrupt_entries[irq].type != INTERRUPT_NONE)
        return 0;

    interrupt_entries[irq].type = type;
    interrupt_entries[irq].callback = callback;

    interrupt_unmask(irq);

    return 1;
}

void interrupt_unregister(int irq)
{
    if (irq < 0 || irq > MAX_IRQ_NUMBER)
        return;

    interrupt_mask(irq);

    interrupt_entries[irq].type = INTERRUPT_NONE;
}
