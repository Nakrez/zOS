#include <stdio.h>

int fputs(const char *s, FILE *stream)
{
    int ret = 0;

    while (*s)
    {
        putc(*(s++), stream);
        ++ret;
    }

    return ret;
}
