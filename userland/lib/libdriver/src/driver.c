#include <string.h>

#include <zos/device.h>

#include <driver/driver.h>

static void driver_base_open(struct driver *driver, int mid,
                             struct open_msg *msg)
{
    (void) msg;

    struct msg_response resp;

    resp.req_id = mid;
    resp.ret = 0;

    if (device_send_response(driver->dev_id, (void *)&resp, sizeof (resp)) < 0)
        uprint("Default open failed");
}

static void driver_base_close(char *buf)
{
    (void) buf;

    uprint("Close default");
}

int driver_create(const char *dev_name, int uid, int gid, int perm,
                  struct driver_ops *dev_ops, struct driver *result)
{
    int dev_id;

    result->ops = VFS_OPS_OPEN | VFS_OPS_CLOSE;

    if (dev_ops->open == NULL)
        dev_ops->open = driver_base_open;
    if (dev_ops->close == NULL)
        dev_ops->close = driver_base_close;
    if (dev_ops->read != NULL)
        result->ops |= VFS_OPS_READ;
    if (dev_ops->write != NULL)
        result->ops |= VFS_OPS_WRITE;

    dev_id = device_create(dev_name, uid, gid, perm, result->ops);

    if (dev_id < 0)
        return dev_id;

    result->dev_name = dev_name;
    result->dev_id = dev_id;
    result->running = 1;
    result->dev_ops = dev_ops;

    return 0;
}

static void dispatch(struct driver *driver, int mid, char *buf)
{
    int op = mid & 0xFF;

    if (!(op & driver->ops))
    {
        /* TODO */
        uprint("Not supported");
    }

    switch (op)
    {
        case VFS_OPS_OPEN:
            driver->dev_ops->open(driver, mid, (void *)buf);
            break;
    }
}

int driver_loop(struct driver *driver)
{
    int res;
    char buf[255];

    while (driver->running)
    {
        res = device_recv_request(driver->dev_id, buf, 255);

        if (res < 0)
        {
            uprint("Unexpected error in driver_loop()");

            driver->running = 0;

            continue;
        }

        dispatch(driver, res, buf);
    }

    return 0;
}

