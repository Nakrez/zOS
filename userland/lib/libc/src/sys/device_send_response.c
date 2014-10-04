#include <zos/device.h>

#include <arch/syscall.h>

int device_send_response(int dev, uint32_t req_id, void *buf, size_t size)
{
    int ret;

    SYSCALL4(SYS_DEVICE_SEND_RESPONSE, dev, req_id, buf, size, ret);

    return ret;
}
