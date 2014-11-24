#include <stdio.h>

int puts(const char *s)
{
    int ret = fputs(s, stdout);
    fputc('\n', stdout);

    return ret + 1;
}
