#include <string.h>

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && *s2 && *s1 == *s2)
    {
        ++s1;
        ++s2;
        --n;
    }

    if (!n)
        return 0;

    if (*s1 > *s2)
        return 1;

    if (*s1 < *s2)
        return -1;

    return 0;
}
