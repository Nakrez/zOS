#include <string.h>

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
