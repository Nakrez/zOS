#include <unistd.h>
#include <fcntl.h>

#include "iobuffer.h"

int iob_flush(struct _IO_FILE *file)
{
    if (!file)
    {
        spinlock_lock(&_IO_lock);

        struct _IO_FILE *tmp = _IO_files;

        while (tmp)
        {
            if (tmp->fd >= 0 && tmp->flags & O_WRONLY)
                iob_flush(tmp);

            tmp = tmp->next;
        }

        spinlock_unlock(&_IO_lock);

        return 0;
    }

    if (file->fd >= 0 && file->flags & O_WRONLY)
    {
        if (file->_io_write_ptr == file->_io_write_base)
            return 0;

        int ret;

        ret = write(file->fd, file->_io_write_base,
                    file->_io_write_ptr - file->_io_write_base);

        file->_io_write_ptr = file->_io_write_base;

        return ret;
    }

    return -1;
}
