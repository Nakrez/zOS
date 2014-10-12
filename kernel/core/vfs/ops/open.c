#include <string.h>

#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/fs.h>
#include <kernel/vfs/vdevice.h>

int vfs_open(const char *path, int uid, int gid, int flags, mode_t mode)
{
    int ret;
    int fd;
    int path_size = strlen(path);
    struct resp_lookup res;
    struct mount_entry *mount_pt;
    struct process *process = thread_current()->parent;

    if ((ret = vfs_lookup(path, uid, gid, &res, &mount_pt)) < 0)
        return ret;

    if (!mount_pt->ops->open)
        return -ENOSYS;

    /* FIXME: Allow file creation */
    if (ret != path_size)
        return -ENOENT;

    if ((fd = process_new_fd(process)) < 0)
        return fd;

    if (res.dev < 0)
        ret = mount_pt->ops->open(mount_pt, res.inode, uid, gid, flags, mode);
    else
        ret = device_open(res.dev, -1, uid, gid, flags, mode);

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
