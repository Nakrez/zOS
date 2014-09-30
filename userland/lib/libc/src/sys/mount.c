#include <sys/mount.h>

#include <arch/syscall.h>

int mount(int fd, const char *target)
{
    int ret;

    SYSCALL2(SYS_MOUNT, fd, target, ret);

    return ret;
}
