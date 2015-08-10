#include <unistd.h>

#include <arch/syscall.h>

int dup(int oldfd)
{
    int ret;

    SYSCALL1(SYS_DUP, oldfd, ret);

    return ret;
}
