#include <unistd.h>
#include <sys/stat.h>

#include <arch/syscall.h>

int fstat(int fd, struct stat *buf)
{
    int ret;

    SYSCALL2(SYS_FSTAT, fd, buf, ret);

    return ret;
}
