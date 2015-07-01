#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/device.h>
#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/mount.h>

int vfs_open(struct thread *t, const char *path, int flags, mode_t mode)
{
    int ret;
    int fd;
    int path_size = strlen(path);
    struct file *file;
    struct resp_lookup res;
    struct mount_entry *mount_pt;
    struct process *process;
    struct inode *inode;
    ino_t inode_open;
    uid_t uid;
    gid_t gid;

    /* Kernel request */
    if (!t) {
        uid = 0;
        gid = 0;

        process = process_get(0);
    } else {
        uid = t->uid;
        gid = t->gid;

        process = t->parent;
    }

    ret = vfs_lookup(t, path, &res, &mount_pt);
    if (ret < 0)
        return ret;

    /* FIXME: Allow file creation */
    if (ret != path_size)
        return -ENOENT;

    inode = kmalloc(sizeof (struct inode));
    if (!inode)
        return -ENOMEM;

    if ((fd = process_new_fd(process)) < 0)
        return fd;

    file = &process->files[fd];

    file->inode = inode;
    file->offset = 0;

    inode->mode = mode;

    if (res.inode.dev < 0) {
        file->mount = mount_pt;
        file->f_ops = mount_pt->f_ops;

        inode->dev = mount_pt->dev;

        inode_open = res.inode.inode;
    } else {
        struct device *device;

        device = device_get(res.inode.dev);

        file->mount = NULL;
        file->f_ops = device->f_ops;

        inode->dev = res.inode.dev;

        inode_open = -1;
    }

    if (!file->f_ops->open) {
        ret = -ENOSYS;
        goto error;
    }

    ret = file->f_ops->open(file, inode_open, process->pid, uid, gid, flags,
                            mode);
    if (ret < 0)
        goto error;

    inode->inode = ret;

    return fd;

error:
    process_free_fd(process, fd);
    kfree(inode);
    return ret;
}
