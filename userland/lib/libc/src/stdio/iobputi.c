#include <stdio.h>

#include "iobuffer.h"

int iob_puti(struct _IO_FILE *stream, int num, unsigned int base)
{
    if (num < 0)
    {
        iob_putc('-', stream);

        return 1 + iob_putu(stream, -num, base);
    }

    return iob_putu(stream, num, base);
}
