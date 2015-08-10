#include <string.h>

void *memset(void *ptr, int value, size_t num)
{
    char *mem = ptr;

    for (size_t i = 0; i < num; ++i)
        *(mem++) = value;

    return ptr;
}
