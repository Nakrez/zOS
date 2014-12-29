#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
    const char *c1 = s1;
    const char *c2 = s2;

    while (n)
    {
        if (*c1 != *c2)
            return *c1 - *c2;

        ++c1;
        ++c2;
        --n;
    }

    return 0;
}
