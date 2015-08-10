#include <sys/mount.h>

#include <arch/syscall.h>

int mount(dev_t dev, const char *target)
{
    int ret;

    SYSCALL2(SYS_MOUNT, dev, target, ret);

    return ret;
}
