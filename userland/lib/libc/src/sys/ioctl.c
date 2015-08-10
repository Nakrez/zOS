#include <sys/ioctl.h>

#include <arch/syscall.h>

int ioctl(int fd, int req, int *argp)
{
    int ret;

    SYSCALL3(SYS_IOCTL, fd, req, argp, ret);

    return ret;
}
