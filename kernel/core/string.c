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
    char *res = destination;

    while (*source)
        *(destination++) = *(source++);

    *destination = 0;

    return res;
}

int strcmp(const char *str1, const char *str2)
{
    while (*str1 && *str2 && *str1 == *str2)
    {
        ++str1;
        ++str2;
    }

    if (*str1 > *str2)
        return 1;

    if (*str1 < *str2)
        return -1;

    return 0;
}
