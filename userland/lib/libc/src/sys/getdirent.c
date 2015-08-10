#include <dirent.h>

#include <arch/syscall.h>

int getdirent(int fd, struct dirent *d, int index)
{
    int ret;

    SYSCALL3(SYS_GETDIRENT, fd, d, index, ret);

    return ret;
}
