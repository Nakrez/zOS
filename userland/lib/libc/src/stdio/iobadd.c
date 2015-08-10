#include <unistd.h>

#include "iobuffer.h"

void iob_add(struct _IO_FILE *file)
{
    spinlock_lock(&_IO_lock);

    if (!_IO_files)
    {
        _IO_files = file;

        file->prev = NULL;
        file->next = NULL;
    }
    else
    {
        file->next = _IO_files;
        file->prev = NULL;

        _IO_files->prev = file;
        _IO_files = file;
    }

    spinlock_unlock(&_IO_lock);
}
