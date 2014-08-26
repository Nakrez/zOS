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

size_t strlen(const char *str)
{
    size_t size = 0;

    while (*(str++))
        ++size;

    return size;
}

char *strcpy(char *destination, const char *source)
{
    while (*source)
        *(destination++) = *(source++);

    return destination;
}
