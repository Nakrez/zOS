#include <unistd.h>

#include <arch/syscall.h>

int close(int fd)
{
    int ret;

    SYSCALL1(SYS_CLOSE, fd, ret);

    return ret;
}
