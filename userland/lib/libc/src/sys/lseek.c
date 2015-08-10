#include <unistd.h>

#include <arch/syscall.h>

int lseek(int fd, off_t off, int whence)
{
    int ret;
    uint32_t low = off & 0xFFFFFFFF;
    uint32_t high = off >> 32;

    SYSCALL4(SYS_LSEEK, fd, high, low, whence, ret);

    return ret;
}
