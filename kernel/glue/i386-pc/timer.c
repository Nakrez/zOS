#include <kernel/timer.h>
#include <kernel/interrupt.h>
#include <kernel/console.h>

#include <glue/timer.h>

#include <arch/pit.h>
#include <arch/pic.h>

struct timer_glue timer_glue_dispatcher =
{
    i386_pc_timer_initialize,
};

int i386_pc_timer_initialize(void)
{
    int err;

    pit_initialize();

    err = interrupt_register(PIC_IRQ_PIT, INTERRUPT_CALLBACK, timer_handler);
    if (err < 0) {
        console_message(T_ERR, "Unable to register timer handler");
        return err;
    }

    return 0;
}
