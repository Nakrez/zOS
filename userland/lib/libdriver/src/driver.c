#include <string.h>
#include <stdlib.h>

#include <zos/device.h>
#include <zos/print.h>

#include <driver/driver.h>

static int driver_base_open(struct driver *driver, int mid,
                            struct req_open *request, ino_t *inode)
{
    (void) driver;
    (void) mid;
    (void) request;

    *inode = 0;

    return 0;
}

static int driver_base_close(struct driver *driver, int mid,
                             struct req_close *request)
{
    (void) driver;
    (void) mid;
    (void) request;

    return 0;
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

    if (!(result->dev_name = malloc(strlen(dev_name) + 1)))
        return -1;

    dev_id = device_create(dev_name, uid, gid, perm, result->ops);

    if (dev_id < 0)
    {
        free(result->dev_name);

        return dev_id;
    }

    result->dev_name = strcpy(result->dev_name, dev_name);
    result->dev_id = dev_id;
    result->running = 1;
    result->dev_ops = dev_ops;

    return 0;
}

static void dispatch(struct driver *driver, int mid, char *buf)
{
    uint8_t op = mid & 0xFF;

    switch (op)
    {
        case VFS_OPS_OPEN:
            {
                struct resp_open response;

                response.ret = driver->dev_ops->open(driver, mid, (void *)buf,
                                                     &response.inode);

                device_send_response(driver->dev_id, mid, &response,
                                     sizeof (struct resp_open));
            }
            break;
        case VFS_OPS_READ:
            {
                struct resp_rdwr response;

                response.ret = driver->dev_ops->read(driver, mid, (void *)buf,
                                                     &response.size);

                device_send_response(driver->dev_id, mid, &response,
                                     sizeof (struct resp_rdwr));
            }
            break;
        case VFS_OPS_WRITE:
            {
                struct resp_rdwr response;

                response.ret = driver->dev_ops->write(driver, mid, (void *)buf,
                                                      &response.size);

                device_send_response(driver->dev_id, mid, &response,
                                     sizeof (struct resp_rdwr));
            }
            break;
        case VFS_OPS_CLOSE:
            {
                struct resp_close response;

                response.ret = driver->dev_ops->close(driver, mid,
                                                      (void *)buf);

                device_send_response(driver->dev_id, mid, &response,
                                     sizeof (struct resp_close));
            }
            break;
        default:
            uprint("Not supported");
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
