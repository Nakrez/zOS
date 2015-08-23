#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/mount.h>

int vfs_getdirent(struct thread *t, int fd, struct dirent *dirent, int index)
{
    int ret;
    struct process *p;
    struct file *file;

    if (!t)
        p = process_get(0);
    else
        p = t->parent;

    ret = process_file_from_fd(p, fd, &file);
    if (ret < 0)
        return ret;

    if (!p->files[fd].mount->fs_ops->getdirent)
        return -ENOSYS;

    return p->files[fd].mount->fs_ops->getdirent(p->files[fd].mount,
                                                 p->files[fd].inode->inode,
                                                 dirent, index);
}
