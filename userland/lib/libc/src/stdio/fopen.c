#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "iobuffer.h"

FILE *fopen(const char *filename, const char *mode)
{
    int flags = 0;
    int fd;

    while (*mode)
    {
        switch (*mode)
        {
            case 'r':
                flags |= O_RDONLY;
                break;
            case 'w':
                flags |= O_WRONLY;
                break;
            default:
                return NULL;
        }

        ++mode;
    }

    if ((fd = open(filename, flags, 0)) < 0)
        return NULL;

    return iob_create(fd, flags);
}
