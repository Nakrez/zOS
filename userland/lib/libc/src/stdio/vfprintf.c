#include <stdio.h>

#include "iobuffer.h"

int vfprintf(FILE *stream, const char *format, va_list ap)
{
    int ret = 0;

    while (*format)
    {
        if (*format == '%')
        {
            ++format;

            switch (*format)
            {
                case 'i':
                case 'd':
                    ret += iob_puti(stream, va_arg(ap, int), 10);
                    break;
                case 'u':
                    ret += iob_putu(stream, va_arg(ap, unsigned), 10);
                    break;
                case 'x':
                    ret += iob_putu(stream, va_arg(ap, unsigned), 16);
                    break;
                case 'p':
                    iob_putc('0', stream);
                    iob_putc('x', stream);
                    ret += 2 + iob_putu(stream, va_arg(ap, uintptr_t), 16);
                    break;
                case 'o':
                    ret += iob_puti(stream, va_arg(ap, int), 8);
                    break;
                case 's':
                    ret += fputs(va_arg(ap, const char *), stream);
                    break;
                default:
                    fputc(*format, stream);
                    ++ret;
                    break;
            }
        }
        else
        {
            fputc(*format, stream);
            ++ret;
        }

        ++format;
    }

    return ret;
}
