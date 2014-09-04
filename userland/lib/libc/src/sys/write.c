#include <unistd.h>

#include <arch/syscall.h>

int write(int fd, const void *buf, size_t count)
{
    int ret;

    SYSCALL3(SYS_WRITE, fd, buf, count, ret);

    return ret;
}
