#include <zos/device.h>

#include <arch/syscall.h>

int device_create(const char *name, int uid, int gid, int perm, int ops)
{
    int ret;

    SYSCALL5(SYS_DEVICE_CREATE, name, uid, gid, perm, ops, ret);

    return ret;
}
