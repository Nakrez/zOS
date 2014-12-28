#include "utils.h"

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

int strlen(const char *s)
{
    const char *base = s;

    while (*s)
        ++s;

    return s - base;
}
