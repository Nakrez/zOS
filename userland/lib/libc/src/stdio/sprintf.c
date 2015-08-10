#include <stdio.h>

int sprintf(char *str, const char *format, ...)
{
    int ret;
    va_list ap;

    va_start(ap, format);

    ret = vsprintf(str, format, ap);

    va_end(ap);

    return ret;
}
