#include <unistd.h>

#include <arch/syscall.h>

int read(int fd, void *buf, size_t count)
{
    int ret;

    SYSCALL3(SYS_READ, fd, buf, count, ret);

    return ret;
}
