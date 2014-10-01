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

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    char *base = str;
    const char *delim_base = delim;

    if (!str && !(**saveptr))
        return NULL;

    if (base == NULL)
    {
        base = *saveptr;
        str = *saveptr;
    }

    while (*str)
    {
        while (*delim)
        {
            if (*delim == *str)
            {
                *str = 0;

                break;
            }

            ++delim;
        }

        ++str;

        if (*delim)
            break;

        delim = delim_base;
    }

    *saveptr = str;

    return base;
}

char *strchr(const char *s, int c)
{
    while (*s)
    {
        if (*s == c)
            return (char *)s;

        ++s;
    }

    return NULL;
}
