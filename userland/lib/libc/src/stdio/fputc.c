#include <stdio.h>

#include "iobuffer.h"

int fputc(int c, FILE *stream)
{
    return iob_putc(c, stream);
}
