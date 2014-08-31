#include <string.h>

void *memcpy(void *dest, const void *src, size_t size)
{
    void *d = dest;

    while (size--)
        *((char *)(dest++)) = *((char *)(src++));

    return d;
}
