#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/vdevice.h>
#include <kernel/fs/vfs/message.h>

int vfs_open(struct thread *t, const char *path, int flags, mode_t mode)
{
    int ret;
    int fd;
    int path_size = strlen(path);
    struct resp_lookup res;
    struct mount_entry *mount_pt;
    struct process *process;
    uid_t uid;
    gid_t gid;

    /* Kernel request */
    if (!t)
    {
        uid = 0;
        gid = 0;

        process = process_get(0);
    }
    else
    {
        uid = t->uid;
        gid = t->gid;

        process = t->parent;
    }

    if ((ret = vfs_lookup(t, path, &res, &mount_pt)) < 0)
        return ret;

    if (!mount_pt->ops->open)
        return -ENOSYS;

    /* FIXME: Allow file creation */
    if (ret != path_size)
        return -ENOENT;

    if ((fd = process_new_fd(process)) < 0)
        return fd;

    if (res.dev < 0)
        ret = mount_pt->ops->open(mount_pt, res.inode, process->pid, uid, gid,
                                  flags, mode);
    else
        ret = device_open(res.dev, -1, process->pid, uid, gid, flags, mode);

    if (ret < 0)
    {
        process_free_fd(process, fd);

        return ret;
    }

    process->files[fd].offset = 0;
    process->files[fd].mode = mode;
    process->files[fd].inode = ret;

    if (res.dev < 0)
    {
        process->files[fd].mount = mount_pt;
        process->files[fd].dev = -1;
    }
    else
    {
        process->files[fd].mount = NULL;
        process->files[fd].dev = res.dev;
    }

    return fd;
}
