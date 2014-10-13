#include <string.h>

#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/fs.h>
#include <kernel/vfs/vdevice.h>

int vfs_close(struct thread *t, int fd)
{
    int ret;
    struct process *p;

    /* Kernel request */
    if (!t)
        p = process_get(0);
    else
        p = thread_current()->parent;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD || !p->files[fd].used)
        return -EINVAL;

    if (p->files[fd].dev >= 0)
        ret = device_close(p->files[fd].dev, p->files[fd].inode);
    else
        ret = p->files[fd].mount->ops->close(p->files[fd].mount,
                                             p->files[fd].inode);

    if (ret < 0)
        return ret;

    process_free_fd(p, fd);

    return 0;
}
