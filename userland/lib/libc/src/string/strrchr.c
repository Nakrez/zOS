#include <string.h>

char *strrchr(const char *s, int c)
{
    char *res = NULL;

    while (*s)
    {
        if (*s == c)
            res = (char *)s;

        ++s;
    }

    return res;
}
