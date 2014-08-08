#include <kernel/timer.h>

#include <glue/timer.h>

#include <arch/pit.h>

struct timer_glue __timer =
{
    i386_pc_timer_initialize,
};

void i386_pc_timer_initialize(void)
{
    pit_initialize();
}
