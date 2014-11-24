#include <stdlib.h>

#include <zos/device.h>
#include <zos/print.h>

#include <sys/mount.h>

#include <fiu/fiu.h>

static int fiu_capabilities(struct fiu_internal *fiu)
{
    fiu->capabilities = 0;

    /* Lookup is a mandatory operation */
    if (fiu->ops->lookup)
        fiu->capabilities |= VFS_OPS_LOOKUP;
    else
        return -1;

    if (fiu->ops->stat)
        fiu->capabilities |= VFS_OPS_STAT;

    if (fiu->ops->open)
        fiu->capabilities |= VFS_OPS_OPEN;
    else
        return -1;

    if (fiu->ops->read)
        fiu->capabilities |= VFS_OPS_READ;

    if (fiu->ops->close)
        fiu->capabilities |= VFS_OPS_CLOSE;
    else
        return -1;

    return 0;
}

int fiu_create(const char *name, int perm, struct fiu_ops *ops,
               struct fiu_internal *fiu)
{
    fiu->ops = ops;
    fiu->dev_id = -1;
    fiu->running = 0;

    if (fiu_capabilities(fiu) < 0)
        return -1;

    fiu->dev_id = device_create(name, perm, fiu->capabilities);

    if (fiu->dev_id < 0)
        return fiu->dev_id;

    return 0;
}

static void fiu_dispatch(struct fiu_internal *fiu, int mid, char *buf)
{
    uint8_t op = mid & 0xFF;

    switch (op)
    {
        case 0:
            if (fiu->ops->root_remount)
                fiu->ops->root_remount(fiu, (void *)buf);
            else
                uprint("root_remount() was issued but not catched");
            break;
        case VFS_LOOKUP:
            {
                struct resp_lookup resp;

                resp.ret = fiu->ops->lookup(fiu, (void *)buf, &resp);

                device_send_response(fiu->dev_id, mid, &resp,
                                     sizeof (struct resp_lookup));
            }
            break;
        case VFS_STAT:
            {
                struct resp_stat resp;

                resp.ret = fiu->ops->stat(fiu, (void *)buf, &resp.stat);

                device_send_response(fiu->dev_id, mid, &resp,
                                     sizeof (struct resp_stat));
            }
            break;
        case VFS_OPEN:
            {
                struct resp_open resp;

                resp.ret = fiu->ops->open(fiu, (void *)buf, &resp);

                device_send_response(fiu->dev_id, mid, &resp,
                                     sizeof (struct resp_open));
            }
            break;
        case VFS_READ:
            {
                struct resp_rdwr resp;

                resp.ret = fiu->ops->read(fiu, (void *)buf, &resp.size);

                device_send_response(fiu->dev_id, mid, &resp,
                                     sizeof (struct resp_rdwr));
            }
            break;
        case VFS_CLOSE:
            {
                struct resp_close resp;

                resp.ret = fiu->ops->close(fiu, (void *)buf);

                device_send_response(fiu->dev_id, mid, &resp,
                                     sizeof (struct resp_close));
            }
            break;
        default:
            uprint("fiu_dispatch(): Unsupported operation");
            break;
    }
}

static int fiu_loop(struct fiu_internal *fiu)
{
    int res = 0;
    char *buf = malloc(255);

    if (!buf)
        return 1;

    fiu->running = 1;

    while (fiu->running > 0)
    {
        res = device_recv_request(fiu->dev_id, buf, 255);

        if (res < 0)
        {
            uprint("Unexpected error in fiu_loop()");

            fiu->running = 0;

            continue;
        }

        fiu_dispatch(fiu, res, buf);
    }

    return res;
}

int fiu_main(struct fiu_internal *fiu, const char *mount_path)
{
    int ret;

    if (fiu->dev_id < 0)
        return fiu->dev_id;

    ret = mount(fiu->dev_id, mount_path);

    if (ret < 0)
        return ret;

    return fiu_loop(fiu);
}
