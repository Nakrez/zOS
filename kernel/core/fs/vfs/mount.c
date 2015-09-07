#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/fiu.h>

#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/device.h>
#include <kernel/fs/vfs/mount.h>
#include <kernel/fs/vfs/message.h>

#include <arch/spinlock.h>

# define MAX_MOUNTED_PATH 5

static struct mount_entry mounts[MAX_MOUNTED_PATH];
static spinlock_t mount_lock = SPINLOCK_INIT;

static int vfs_check_mounts(const char *mount_path)
{
    int mount_nb = -1;

    spinlock_lock(&mount_lock);

    for (int i = 0; i < MAX_MOUNTED_PATH; ++i)
    {
        if (!mounts[i].used) {
            if (mount_nb == -1)
                mount_nb = i;
            continue;
        }

        if (!strcmp(mounts[i].path, mount_path)) {
            spinlock_unlock(&mount_lock);
            return -EBUSY;
        }
    }

    if (mount_nb == -1) {
        spinlock_unlock(&mount_lock);
        return -ENOMEM;
    }

    mounts[mount_nb].used = 1;

    spinlock_unlock(&mount_lock);

    return mount_nb;
}

static int do_mount(struct thread *t, const char *mount_path, int mount_nb)
{
    int ret;
    int path_size;
    struct resp_lookup res;
    struct mount_entry *mount_pt;

    path_size = strlen(mount_path);

    ret = vfs_lookup(t, mount_path, &res, &mount_pt);

    if (ret < 0)
        return ret;

    if (!mount_pt->fi->parent->fs_ops->mount)
        return -ENOSYS;

    if (ret != path_size)
        return -ENOENT;

    return mount_pt->fi->parent->fs_ops->mount(mount_pt, res.inode.inode,
                                               mount_nb);
}

int vfs_mount(struct thread *t, const char *fs_name, const char *device,
              const char *mount_pt)
{
    int ret;
    int mount_nb;
    struct fs *fs;
    struct fs_instance *fi;

    fs = fs_from_name(fs_name);
    if (!fs)
        return -EINVAL;

    mount_nb = vfs_check_mounts(mount_pt);
    if (mount_nb < 0)
        return mount_nb;

    ret = fs_new_instance(fs_name, device, mount_pt, &fi);
    if (ret < 0) {
        mounts[mount_nb].used = 0;
        return ret;
    }

    /* Do mount is used to tell a file system that another one is getting
     * mounted. If we mount root there is no point in calling this function
     */
    if (mount_nb != 0) {
        ret = do_mount(t, mount_pt, mount_nb);
        if (ret < 0) {
            mounts[mount_nb].used = 0;
            return ret;
        }
    }

    mounts[mount_nb].path = kmalloc(strlen(mount_pt) + 1);
    if (!mounts[mount_nb].path) {
        mounts[mount_nb].used = 0;
        return -ENOMEM;
    }

    strcpy(mounts[mount_nb].path, mount_pt);

    mounts[mount_nb].fi = fi;

    return 0;
}

struct mount_entry *vfs_root_get(void)
{
    return &mounts[0];
}

struct mount_entry *vfs_mount_pt_get(const char *path)
{
    for (int i = 0; i < MAX_MOUNTED_PATH; ++i)
    {
        if (mounts[i].used && !strcmp(path, mounts[i].path))
            return &mounts[i];
    }

    return NULL;
}
