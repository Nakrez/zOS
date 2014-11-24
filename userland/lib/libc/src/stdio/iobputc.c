#include <stdio.h>

#include "iobuffer.h"

int iob_putc(int c, struct _IO_FILE *stream)
{
    if (!stream)
        return -1;

    *(stream->_io_write_ptr++) = c;

    if (stream->_io_write_ptr == stream->_io_write_end ||
        (((stream->flags >> 8) & 3) == _IOLBF && c == '\n'))
        iob_flush(stream);

    return c;
}
