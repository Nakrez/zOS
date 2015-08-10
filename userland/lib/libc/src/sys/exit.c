#include <stdio.h>
#include <stdlib.h>

#include <arch/syscall.h>

#include "../stdio/iobuffer.h"

static void close_streams(void)
{
    struct _IO_FILE *tmp = _IO_files;
    struct _IO_FILE *next;

    /* Close streams */

    while (tmp)
    {
        next = tmp->next;

        fclose(tmp);

        tmp = next;
    }
}

void exit(int status)
{
    int ret;

    close_streams();

    SYSCALL1(SYS_EXIT, status, ret);

    while (1)
        ;
}
