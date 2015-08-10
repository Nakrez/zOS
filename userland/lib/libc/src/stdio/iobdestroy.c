#include <unistd.h>
#include <stdlib.h>

#include "iobuffer.h"

void iob_destroy(struct _IO_FILE *file)
{
    if (file->fd >= 0)
    {
        close(file->fd);

        file->fd = -1;
    }

    free(file->_io_read_base);
    free(file->_io_write_base);
}
