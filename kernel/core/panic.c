#include <kernel/panic.h>
#include <kernel/console.h>

void arch_back_trace(void);

void kernel_panic(const char *s)
{
    console_message(T_ERR, "kernel panic: %s", s);

    arch_back_trace();

    for (;;)
        ;
}
