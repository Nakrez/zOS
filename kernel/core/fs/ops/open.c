#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/device.h>
#include <kernel/fs/vfs/message.h>

int vfs_open(struct thread *t, const char *path, int flags, mode_t mode)
{
    int ret;
    int fd;
    int path_size = strlen(path);
    struct file *file;
    struct resp_lookup res;
    struct mount_entry *mount_pt;
    struct process *process;
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

    if ((fd = process_new_fd(process)) < 0)
        return fd;

    file = &process->files[fd];

    file->offset = 0;
    file->mode = mode;

    if (res.dev < 0) {
        file->mount = mount_pt;
        file->inode = res.inode;
        file->dev = mount_pt->dev;
        file->f_ops = mount_pt->f_ops;
    } else {
        struct device *device;

        device = device_get(res.dev);

        file->mount = NULL;
        file->dev = res.dev;
        file->inode = -1;
        file->f_ops = device->f_ops;
    }

    if (!file->f_ops->open) {
        process_free_fd(process, fd);
        return -ENOSYS;
    }

    ret = file->f_ops->open(file, file->inode, process->pid, uid, gid, flags,
                            mode);
    if (ret < 0) {
        process_free_fd(process, fd);
        return ret;
    }

    file->inode = ret;

    return fd;
}
