#include <string.h>

void *memcpy(void *dest, const void *src, size_t size)
{
    while (size--)
        *((char *)dest++) = *((char *)src++);

    return dest;
}

void *memset(void *ptr, int value, size_t size)
{
    while (size--)
        *((char *)ptr++) = value;

    return ptr;
}
