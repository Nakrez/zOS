#include <kernel/console.h>

void console_init(void)
{
    __console.init();

    console_clear();
}

void console_clear(void)
{
    __console.clear();
}

void console_puts(const char *s)
{
    while (*s)
        __console.putc(*(s++));
}
