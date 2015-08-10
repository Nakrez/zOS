#include <string.h>

char *strcpy(char *dest, const char *src)
{
    char *ret = dest;

    while (*src)
        *(dest++) = *(src++);

    *dest = 0;

    return ret;
}
