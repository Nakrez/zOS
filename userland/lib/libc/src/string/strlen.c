#include <string.h>

size_t strlen(const char *s)
{
    const char *begin = s;

    while (*(s++))
        ;

    return s - begin - 1;
}
