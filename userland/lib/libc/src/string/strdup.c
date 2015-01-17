#include <stdlib.h>
#include <string.h>

char *strdup(const char *s)
{
    char *str;
    size_t len = strlen(s);

    if (!(str = malloc(len + 1)))
        return NULL;

    return strcpy(str, s);
}
