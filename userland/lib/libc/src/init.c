#include <stdio.h>
#include <fcntl.h>

#include "stdio/iobuffer.h"
#include "stdlib/init.h"

int __libc_init(void)
{
    if (malloc_initialize() < 0)
        return -1;

    stdin = iob_create(STDIN_FILENO, O_RDONLY | (_IONBF << 8));

    stdout = iob_create(STDOUT_FILENO, O_WRONLY | (_IOLBF << 8));

    stderr = iob_create(STDERR_FILENO, O_WRONLY | (_IONBF << 8));

    return 0;
}
