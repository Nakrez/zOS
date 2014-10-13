#include <zos/device.h>

#include <arch/syscall.h>

dev_t device_create(const char *name, int perm, vop_t ops)
{
    int ret;

    SYSCALL3(SYS_DEVICE_CREATE, name, perm, ops, ret);

    return ret;
}
