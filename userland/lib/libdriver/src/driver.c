#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <zos/device.h>
#include <zos/print.h>

#include <driver/driver.h>

static int driver_base_open(struct driver *driver, struct req_open *request,
                            ino_t *inode)
{
    (void) driver;
    (void) request;

    *inode = 0;

    return 0;
}

static int driver_base_close(struct driver *driver, struct req_close *request)
{
    (void) driver;
    (void) request;

    return 0;
}

int driver_create(const char *dev_name, int perm, struct driver_ops *dev_ops,
                  struct driver *result)
{
    int dev_id;
    int channel_fd;

    result->ops = VFS_OPS_OPEN | VFS_OPS_CLOSE;

    if (dev_ops->open == NULL)
        dev_ops->open = driver_base_open;
    if (dev_ops->close == NULL)
        dev_ops->close = driver_base_close;
    if (dev_ops->read != NULL)
        result->ops |= VFS_OPS_READ;
    if (dev_ops->write != NULL)
        result->ops |= VFS_OPS_WRITE;
    if (dev_ops->ioctl != NULL)
        result->ops |= VFS_OPS_IOCTL;

    result->dev_name = malloc(strlen(dev_name) + 1);
    if (!result->dev_name)
        /* XXX: ENOMEM */
        return -1;

    channel_fd = channel_create(dev_name);
    if (channel_fd < 0)
        return channel_fd;

    dev_id = device_create(channel_fd, dev_name, perm, result->ops);
    if (dev_id < 0) {
        close(channel_fd);
        free(result->dev_name);
        return dev_id;
    }

    result->channel_fd = channel_fd;
    result->dev_name = strcpy(result->dev_name, dev_name);
    result->dev_id = dev_id;
    result->running = 1;
    result->dev_ops = dev_ops;

    return 0;
}

static void dispatch(struct driver *driver, char *buf)
{
    struct msg_header *hdr = (void *) buf;

    switch (hdr->op)
    {
        case VFS_OPEN:
            {
                struct resp_open resp;

                resp.ret = driver->dev_ops->open(driver, (void *)buf,
                                                 &resp.inode);
                if (resp.ret == DRV_NORESPONSE)
                    break;

                resp.hdr.slave_id = hdr->slave_id;

                write(driver->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_READ:
            {
                struct resp_rdwr resp;

                resp.ret = driver->dev_ops->read(driver, (void *)buf,
                                                 &resp.size);
                if (resp.ret == DRV_NORESPONSE)
                    break;

                resp.hdr.slave_id = hdr->slave_id;

                write(driver->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_WRITE:
            {
                struct resp_rdwr resp;

                resp.ret = driver->dev_ops->write(driver, (void *)buf,
                                                  &resp.size);
                if (resp.ret == DRV_NORESPONSE)
                    break;

                resp.hdr.slave_id = hdr->slave_id;

                write(driver->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_CLOSE:
            {
                struct resp_close resp;

                resp.ret = driver->dev_ops->close(driver, (void *)buf);
                if (resp.ret == DRV_NORESPONSE)
                    break;

                resp.hdr.slave_id = hdr->slave_id;

                write(driver->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_IOCTL:
            {
                struct resp_ioctl resp;

                resp.ret = driver->dev_ops->ioctl(driver, (void *)buf, &resp);
                if (resp.ret == DRV_NORESPONSE)
                    break;

                resp.hdr.slave_id = hdr->slave_id;

                write(driver->channel_fd, &resp, sizeof (resp));
            }
            break;
        default:
            {
                char tmp[100];

                sprintf(tmp, "Not supported (%i, %i)", hdr->op, hdr->slave_id);

                uprint(tmp);
            }
            break;
    }
}

int driver_loop(struct driver *driver)
{
    int ret;
    char *buf = malloc(255);

    if (!buf)
        return 1;

    while (driver->running)
    {
        ret = read(driver->channel_fd, buf, 255);
        if (ret < 0) {
            uprint("Unexpected error in driver_loop()");
            driver->running = 0;
            continue;
        }

        dispatch(driver, buf);
    }

    free(buf);

    return 0;
}
