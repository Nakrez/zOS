#include <kernel/timer.h>
#include <kernel/interrupt.h>
#include <kernel/panic.h>

#include <glue/timer.h>

#include <arch/pit.h>
#include <arch/pic.h>

struct timer_glue timer_glue_dispatcher =
{
    i386_pc_timer_initialize,
};

int i386_pc_timer_initialize(void)
{
    pit_initialize();

    if (!interrupt_register(PIC_IRQ_PIT, INTERRUPT_CALLBACK, timer_handler))
        kernel_panic("Unable to register timer handler");

    return 1;
}
