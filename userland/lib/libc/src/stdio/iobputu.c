#include <stdio.h>

#include "iobuffer.h"

int iob_putu(struct _IO_FILE *stream, unsigned int num, unsigned int base)
{
    if (!num)
    {
        iob_putc('0', stream);

        return 1;
    }
    else
    {
        char c = num % base;

        if (num > base)
            iob_putu(stream, num / base, base);

        if (c < 10)
            iob_putc(c + '0', stream);
        else
            iob_putc(c + 'A', stream);

        return 1;
    }
}
