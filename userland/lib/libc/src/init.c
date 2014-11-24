#include <stdio.h>
#include <fcntl.h>

#include "stdio/iobuffer.h"

void __libc_init(void)
{
    stdin = iob_create(STDIN_FILENO, O_RDONLY | (_IONBF << 8));

    stdout = iob_create(STDOUT_FILENO, O_WRONLY | (_IOLBF << 8));

    stderr = iob_create(STDERR_FILENO, O_WRONLY | (_IONBF << 8));
}
