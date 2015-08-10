#include <stdio.h>

int fprintf(FILE *stream, const char *s, ...)
{
    int ret;
    va_list ap;

    va_start(ap, s);

    ret = vfprintf(stream, s, ap);

    va_end(ap);

    return ret;
}
