#include <string.h>

#include <zos/device.h>

#include <driver/driver.h>

void driver_base_open(void)
{
}

void driver_base_close(void)
{
}

int driver_create(const char *dev_name, int uid, int gid, int perm,
                  struct driver_ops *dev_ops, struct driver *result)
{
    int dev_id;
    int ops = VFS_OPS_OPEN | VFS_OPS_CLOSE;

    if (dev_ops->open == NULL)
        dev_ops->open = driver_base_open;
    if (dev_ops->close == NULL)
        dev_ops->close = driver_base_close;
    if (dev_ops->read != NULL)
        ops |= VFS_OPS_READ;
    if (dev_ops->write != NULL)
        ops |= VFS_OPS_WRITE;

    dev_id = device_create(dev_name, uid, gid, perm, ops);

    if (dev_id < 0)
        return dev_id;

    result->dev_name = dev_name;
    result->dev_id = dev_id;
    result->running = 1;
    result->dev_ops = dev_ops;

    return 0;
}
