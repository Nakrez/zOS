#include <kernel/timer.h>
#include <kernel/event.h>
#include <kernel/panic.h>

#include <glue/timer.h>

#include <arch/pit.h>
#include <arch/pic.h>

struct timer_glue __timer =
{
    i386_pc_timer_initialize,
};

void i386_pc_timer_initialize(void)
{
    pit_initialize();

    if (!event_register(PIC_IRQ_PIT, EVENT_CALLBACK, timer_handler))
        kernel_panic("Unable to register timer handler");
}
