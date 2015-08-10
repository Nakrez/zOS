#include <stdlib.h>

#include "iobuffer.h"

void iob_remove(struct _IO_FILE *file)
{
    spinlock_lock(&_IO_lock);

    if (file->next == file->prev)
        _IO_files = NULL;
    else
    {
        if (file->next)
            file->next->prev = file->prev;
        if (file->prev)
            file->prev->next = file->next;

        if (_IO_files == file)
            _IO_files = file->next;
    }

    spinlock_unlock(&_IO_lock);

    free(file);
}
