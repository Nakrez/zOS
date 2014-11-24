#include <stdio.h>

int fputs(const char *s, FILE *stream)
{
    int ret;

    while (*s)
    {
        putc(*(s++), stream);
        ++ret;
    }

    return ret;
}
