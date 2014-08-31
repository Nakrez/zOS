#include <kernel/panic.h>
#include <kernel/console.h>

#include <arch/cpu.h>

void arch_back_trace(void);

void kernel_panic(const char *s)
{
    cpu_irq_disable();

    console_message(T_ERR, "kernel panic: %s", s);

    arch_back_trace();

    for (;;)
        ;
}
