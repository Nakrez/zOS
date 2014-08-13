#include <stdlib.h>
#include <arch/syscall.h>

void exit(int status)
{
    int ret;

    SYSCALL1(SYS_EXIT, status, ret);

    while (1)
        ;
}
