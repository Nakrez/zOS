#include <unistd.h>

#include <arch/syscall.h>

int lseek(int fd, off_t off, int whence)
{
    int ret;

    SYSCALL3(SYS_LSEEK, fd, off, whence, ret);

    return ret;
}
