#include <kernel/zos.h>
#include <kernel/types.h>
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

static void console_unsigned(uint32_t num, uint32_t base)
{
    if (num >= base)
    {
        console_unsigned(num / base, base);
        num %= base;
    }

    if (num >= 10)
        __console.putc('A' + num - 10);
    else
        __console.putc('0' + num);
}

static void console_signed(int32_t num, uint32_t base)
{
    if (num < 0)
    {
        __console.putc('-');
        num = -num;
    }

    console_unsigned(num, base);
}

static void console_vprintf(const char *s, va_list args)
{
    while (*s)
    {
        if (*s == '%')
        {
            ++s;

            if (*s == 'c')
                __console.putc(va_arg(args, int));
            else if (*s == 's')
                console_puts(va_arg(args, char *));
            else if (*s == 'u')
                console_unsigned(va_arg(args, uint32_t), 10);
            else if (*s == 'i')
                console_signed(va_arg(args, int32_t), 10);
            else if (*s == 'x')
                console_unsigned(va_arg(args, uint32_t), 16);
            else if (*s == '%')
                __console.putc('%');
        }
        else
            __console.putc(*s);

        ++s;
    }
}

void console_printf(const char *s, ...)
{
    va_list args;

    va_start(args, s);

    console_vprintf(s, args);

    va_end(args);
}

void console_message(enum console_type t, const char *s, ...)
{
    va_list args;

    __console.color(COLOR_GREY);
    __console.putc('[');

    switch (t)
    {
        case T_INF:
            __console.color(COLOR_BLUE);
            console_puts("IF");
            break;
        case T_ERR:
            __console.color(COLOR_RED);
            console_puts("KO");
            break;
        case T_OK:
            __console.color(COLOR_GREEN);
            console_puts("OK");
            break;
    }

    __console.color(COLOR_GREY);
    __console.putc(']');
    __console.putc(' ');

    va_start(args, s);

    console_vprintf(s, args);

    va_end(args);

    __console.putc('\n');
}
