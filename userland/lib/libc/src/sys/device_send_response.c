#include <zos/device.h>

#include <arch/syscall.h>

int device_send_response(int dev, char *buf, size_t size)
{
    int ret;

    SYSCALL3(SYS_DEVICE_SEND_RESPONSE, dev, buf, size, ret);

    return ret;
}
