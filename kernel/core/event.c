#include <string.h>

#include <kernel/event.h>
#include <kernel/console.h>
#include <kernel/panic.h>

#include <glue/event.h>

static struct event_entry event_entries[MAX_IRQ_NUMBER];

void event_initialize(void)
{
    memset(event_entries, 0, sizeof (event_entries));

    glue_call(event, init);
}

void event_dispatch(struct irq_regs *regs)
{
    /*
     * FIXME: we use reg->irq_num and reg->irq_data that must be here
     * for every architecture ....
     */
    event_acnowledge(regs->irq_num);

    if (regs->irq_num >= MAX_IRQ_NUMBER)
        kernel_panic("Invalid IRQ number");

    if (event_entries[regs->irq_num].type == EVENT_CALLBACK)
        event_entries[regs->irq_num].callback(regs);
    else if (event_entries[regs->irq_num].type == EVENT_MESSAGE)
        kernel_panic("IRQ message not implemented yet");
    else
        console_message(T_INF, "Unhandled IRQ %i fired with data = 0x%x",
                        regs->irq_num, regs->irq_data);

}

void event_acnowledge(int irq)
{
    glue_call(event, acnowledge, irq);
}

int event_register(int irq, int type, void (*callback)(struct irq_regs *))
{
    if (type == EVENT_NONE)
        return 0;

    if (irq < 0 || irq > MAX_IRQ_NUMBER)
        return 0;

    if (event_entries[irq].type != EVENT_NONE)
        return 0;

    if (type == EVENT_MESSAGE)
        kernel_panic("IRQ message not implemented yet");

    event_entries[irq].type = type;
    event_entries[irq].callback = callback;

    event_unmask(irq);

    return 1;
}

void event_unregister(int irq)
{
    if (irq < 0 || irq > MAX_IRQ_NUMBER)
        return;

    event_mask(irq);

    event_entries[irq].type = EVENT_NONE;
}
