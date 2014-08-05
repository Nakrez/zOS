#include <kernel/console.h>

#include <arch/console.h>

struct console_glue __console =
{
    x86_console_init,
    x86_console_putc,
    x86_console_color,
    x86_console_clear,
};
