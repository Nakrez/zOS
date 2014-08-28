#include <zos/device.h>

#include <arch/syscall.h>

int device_create(const char *name, int uid, int gid, int perm)
{
    int ret;

    SYSCALL4(SYS_DEVICE_CREATE, name, uid, gid, perm, ret);

    return ret;
}
