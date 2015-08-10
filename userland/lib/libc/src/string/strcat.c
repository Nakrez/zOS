#include <string.h>

char *strcat(char *dest, const char *str)
{
    char *tmp = dest;

    while (*tmp)
        ++tmp;

    while (*str)
        *(tmp++) = *(str++);

    *tmp = 0;

    return dest;
}
