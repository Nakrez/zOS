#include <zos/device.h>

#include <arch/syscall.h>

int device_exists(const char *name)
{
    int ret;

    SYSCALL1(SYS_DEVICE_EXISTS, name, ret);

    return ret;
}
