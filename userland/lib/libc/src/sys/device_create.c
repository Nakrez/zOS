#include <zos/device.h>

#include <arch/syscall.h>

dev_t device_create(int channel_fd, const char *name, int perm, vop_t ops)
{
    int ret;

    SYSCALL4(SYS_DEVICE_CREATE, channel_fd, name, perm, ops, ret);

    return ret;
}
