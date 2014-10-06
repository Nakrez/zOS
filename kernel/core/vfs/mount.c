#include <string.h>

#include <kernel/errno.h>
#include <kernel/thread.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/vops.h>
#include <kernel/vfs/vdevice.h>
#include <kernel/vfs/fs.h>
#include <kernel/vfs/tmpfs.h>
#include <kernel/vfs/mount.h>

#include <arch/spinlock.h>

# define MAX_MOUNTED_PATH 5

static struct mount_entry mount_points[MAX_MOUNTED_PATH];
static spinlock_t mount_lock = SPINLOCK_INIT;

static int vfs_check_mount_pts(int dev, const char *mount_path)
{
    int mount_nb = -1;

    spinlock_lock(&mount_lock);

    for (int i = 0; i < MAX_MOUNTED_PATH; ++i)
    {
        if (!mount_points[i].used)
        {
            if (mount_nb == -1)
                mount_nb = i;

            continue;
        }

        if (dev != -1 && dev == mount_points[i].dev)
        {
            spinlock_unlock(&mount_lock);

            return -EBUSY;
        }

        if (!strcmp(mount_points[i].path, mount_path))
        {
            /* Remounting root with regular fs when its tmpfs is allowed */
            if (dev != -1 && mount_points[i].dev == -1
                && !strcmp(mount_path, "/"))
                return 0;

            spinlock_unlock(&mount_lock);

            return -EBUSY;
        }
    }

    if (mount_nb == -1)
    {
        spinlock_unlock(&mount_lock);

        return -ENOMEM;
    }

    mount_points[mount_nb].used = 1;

    spinlock_unlock(&mount_lock);

    return mount_nb;
}

static int do_mount(const char *mount_path, int mount_pt_nb)
{
    int ret;
    int path_size;
    struct resp_lookup res;
    struct mount_entry *mount_pt;

    path_size = strlen(mount_path);

    ret = vfs_lookup(mount_path, 0, 0, &res, &mount_pt);

    if (!mount_pt->ops->mount)
        return -ENOSYS;

    if (ret < 0)
        return ret;

    if (ret != path_size)
        return -ENOENT;

    return mount_pt->ops->mount(mount_pt, res.inode, mount_pt_nb);
}

static void vfs_remount_root_message(int dev, const char *path, int mount_nb)
{
    struct vdevice *device;
    struct message *message;
    struct req_root_remount *request;

    if (!(device = device_get(dev)))
        return;

    if (!(message = message_alloc(sizeof (struct req_root_remount))))
        return;

    request = MESSAGE_EXTRACT(struct req_root_remount, message);

    request->mount_pt = mount_nb;

    strcpy(request->path, path);

    message->mid = message->mid & ~0xFF;

    channel_send_request(device->channel, message);

    message_free(message);
}

int vfs_mount(int dev, const char *mount_path)
{
    int ret;
    int mount_nb;
    struct fs_ops *ops;

    if (dev == TMPFS_DEV_ID)
        ops = &tmpfs_ops;
    else if (!device_get(dev))
        return -EBADF;
    else
        ops = &fiu_ops;

    if ((mount_nb = vfs_check_mount_pts(dev, mount_path)) < 0)
        return mount_nb;

    /* We mount root */
    if (mount_nb == 0)
    {
        if (dev != -1)
        {
            for (int i = 1; i < MAX_MOUNTED_PATH; ++i)
            {
                if (!mount_points[i].used)
                    continue;

                vfs_remount_root_message(dev, mount_points[i].path, i);
            }
        }
    }
    else if ((ret = do_mount(mount_path, mount_nb)) < 0)
        return ret;

    if (ops->init)
    {
        if (!(mount_points[mount_nb].private = ops->init()))
        {
            mount_points[mount_nb].used = 0;

            return -ENOMEM;
        }
    }

    if (!(mount_points[mount_nb].path = kmalloc(strlen(mount_path) + 1)))
    {
        if (ops->cleanup)
            ops->cleanup(mount_points[mount_nb].private);

        mount_points[mount_nb].used = 0;

        return -ENOMEM;
    }

    strcpy(mount_points[mount_nb].path, mount_path);

    mount_points[mount_nb].ops = ops;
    mount_points[mount_nb].dev = dev;

    return 0;
}

struct mount_entry *vfs_root_get(void)
{
    return &mount_points[0];
}

struct mount_entry *vfs_mount_pt_get(const char *path)
{
    for (int i = 0; i < MAX_MOUNTED_PATH; ++i)
    {
        if (mount_points[i].used && !strcmp(path, mount_points[i].path))
            return &mount_points[i];
    }

    return NULL;
}
