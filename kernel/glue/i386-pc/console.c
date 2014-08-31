#include <kernel/console.h>

#include <arch/console.h>

struct console_glue console_glue_dispatcher =
{
    x86_console_init,
    x86_console_putc,
    x86_console_color,
    x86_console_clear,
};
