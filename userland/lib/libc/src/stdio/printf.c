#include <stdio.h>

int printf(const char *s, ...)
{
    int ret;
    va_list ap;

    va_start(ap, s);

    ret = vfprintf(stdout, s, ap);

    va_end(ap);

    return ret;
}
