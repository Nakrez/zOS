#include <stdio.h>

#include "iobuffer.h"

int fflush(FILE *stream)
{
    return iob_flush(stream);
}
