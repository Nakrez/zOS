#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "iobuffer.h"

struct _IO_FILE *iob_create(int fd, int flags)
{
    struct _IO_FILE *file = malloc(sizeof (struct _IO_FILE));

    if (!file)
        return NULL;

    file->_io_read_ptr = NULL;
    file->_io_read_end = NULL;
    file->_io_read_base = NULL;
    file->_io_write_ptr = NULL;
    file->_io_write_end = NULL;
    file->_io_write_base = NULL;

    if (flags & O_RDONLY)
    {
        if (!(file->_io_read_base = malloc(_IO_BUFSIZE_READ)))
        {
            free(file);

            return NULL;
        }

        file->_io_read_ptr = file->_io_read_base;
        file->_io_read_end = file->_io_read_base;
    }

    if (flags & O_WRONLY)
    {
        if (!(file->_io_write_base = malloc(_IO_BUFSIZE_WRITE)))
        {
            free(file->_io_read_base);
            free(file);

            return NULL;
        }

        file->_io_write_ptr = file->_io_write_base;
        file->_io_write_end = file->_io_write_base + _IO_BUFSIZE_WRITE;
    }

    file->fd = fd;
    file->flags = 0;

    file->flags = flags;

    iob_add(file);

    return file;
}
