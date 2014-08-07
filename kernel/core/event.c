#include <kernel/event.h>
#include <kernel/console.h>

void event_initialize(void)
{
    __event.init();
}

void event_dispatch(struct irq_regs *regs)
{
    /*
     * FIXME: we use reg->irq_num and reg->irq_data that must be here
     * for every architecture ....
     */
    console_message(T_INF, "Unhandled IRQ %i fired with data = 0x%x",
                    regs->irq_num, regs->irq_data);
    while (1);
}
