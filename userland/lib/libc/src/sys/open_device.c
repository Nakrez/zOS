#include <zos/vfs.h>

#include <arch/syscall.h>

int open_device(const char *device_name, int flags, mode_t mode)
{
    int ret;

    SYSCALL3(SYS_OPEN_DEVICE, device_name, flags, mode, ret);

    return ret;
}
