#include <fcntl.h>

#include <arch/syscall.h>

int open(const char *pathname, int flags, int mode)
{
    int res;

    SYSCALL3(SYS_OPEN, pathname, flags, mode, res);

    return res;
}
