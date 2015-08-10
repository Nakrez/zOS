#include <unistd.h>

#include <arch/syscall.h>

int dup2(int oldfd, int newfd)
{
    int ret;

    SYSCALL2(SYS_DUP2, oldfd, newfd, ret);

    return ret;
}
