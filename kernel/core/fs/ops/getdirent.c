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

    file = &p->files[fd];

    if (!file->used)
        return -EBADF;

    if (!file->mount->fi->parent->fs_ops->getdirent)
        return -ENOSYS;

    return file->mount->fi->parent->fs_ops->getdirent(file->mount,
                                                      file->inode->inode,
                                                      dirent, index);
}
