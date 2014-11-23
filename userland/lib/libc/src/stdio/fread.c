#include <stdio.h>

#include "iobuffer.h"

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    fflush(stream);

    return iob_read(ptr, size * nmemb, stream);
}
