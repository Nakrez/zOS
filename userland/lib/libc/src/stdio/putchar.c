#include <stdio.h>

#include "iobuffer.h"

int putchar(int c)
{
    return iob_putc(c, stdout);
}
