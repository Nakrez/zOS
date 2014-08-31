#include <zos/device.h>

#include <arch/syscall.h>

int device_recv_request(int dev, char *buf, size_t size)
{
    int ret;

    SYSCALL3(SYS_DEVICE_RECV_REQUEST, dev, buf, size, ret);

    return ret;
}
