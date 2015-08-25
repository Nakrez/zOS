#include <kernel/zos.h>
#include <kernel/types.h>
#include <kernel/console.h>

void console_init(void)
{
    glue_call(console, init);

    console_clear();
}

void console_clear(void)
{
    glue_call(console, clear);
}

void console_puts(const char *s)
{
    while (*s)
        glue_call(console, putc, *(s++));
}

static void console_unsigned(uint32_t num, uint32_t base)
{
    if (num >= base)
    {
        console_unsigned(num / base, base);
        num %= base;
    }

    if (num >= 10)
        glue_call(console, putc, 'A' + num - 10);
    else
        glue_call(console, putc, '0' + num);
}

static void console_signed(int32_t num, uint32_t base)
{
    if (num < 0)
    {
        glue_call(console, putc, '-');
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
                glue_call(console, putc, va_arg(args, int));
            else if (*s == 's')
                console_puts(va_arg(args, char *));
            else if (*s == 'u')
                console_unsigned(va_arg(args, uint32_t), 10);
            else if (*s == 'i' || *s == 'd')
                console_signed(va_arg(args, int32_t), 10);
            else if (*s == 'x')
                console_unsigned(va_arg(args, uint32_t), 16);
            else if (*s == '%')
                glue_call(console, putc, '%');
        }
        else
            glue_call(console, putc, *s);

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

    glue_call(console, color, COLOR_GREY);
    glue_call(console, putc, '[');

    switch (t)
    {
        case T_INF:
            glue_call(console, color, COLOR_BLUE);
            console_puts("IF");
            break;
        case T_ERR:
            glue_call(console, color, COLOR_RED);
            console_puts("KO");
            break;
        case T_OK:
            glue_call(console, color, COLOR_GREEN);
            console_puts("OK");
            break;
    }

    glue_call(console, color, COLOR_GREY);
    glue_call(console, putc, ']');
    glue_call(console, putc, ' ');

    va_start(args, s);

    console_vprintf(s, args);

    va_end(args);

    glue_call(console, putc, '\n');
}
