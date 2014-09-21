#include <stdio.h>
#include <stdint.h>

static void rec_print_u32(char **str, uint32_t num, int base)
{
    char digit = num % base;

    if (num >= base)
        rec_print_u32(str, num / base, base);

    if (digit < 10)
        *((*str)++) = '0' + digit;
    else
        *((*str)++) = 'A' + (digit - 10);
}

static void print_u32(char **str, uint32_t num, int base)
{
    if (num == 0)
        *((*str)++) = '0';
    else
        rec_print_u32(str, num, base);
}

static void print_int(char **str, int num, int base)
{
    char *buf = *str;

    if (num < 0)
    {
        *(buf++) = '-';
        num = -num;
    }

    print_u32(&buf, num, base);

    *str = buf;
}

int vsprintf(char *str, const char *format, va_list ap)
{
    char *begin = str;

    while (*format)
    {
        if (*format == '%')
        {
            ++format;

            switch (*format)
            {
                case 'i':
                case 'd':
                    print_int(&str, va_arg(ap, int), 10);
                    break;

                case '%':
                    *(str++) = '%';
                    break;
            }
        }
        else
            *(str++) = *format;

        ++format;
    }

    *str = 0;

    return str - begin;
}
