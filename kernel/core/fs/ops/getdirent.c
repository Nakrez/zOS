#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/fs.h>

int vfs_getdirent(struct thread *t, int fd, struct dirent *dirent, int index)
{
    struct process *p;

    if (!t)
        p = process_get(0);
    else
        p = t->parent;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD || !p->files[fd].used)
        return -EINVAL;

    if (!p->files[fd].mount->ops->getdirent)
        return -ENOSYS;

    return p->files[fd].mount->ops->getdirent(p->files[fd].mount,
                                              p->files[fd].inode, dirent,
                                              index);
}
