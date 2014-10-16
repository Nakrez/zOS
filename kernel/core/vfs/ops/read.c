#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/vfs.h>
#include <kernel/vfs/fs.h>
#include <kernel/vfs/vdevice.h>

int vfs_read(struct thread *t, int fd, void *buf, size_t count)
{
    int ret;
    struct process *p;
    struct req_rdwr request;

    /* Kernel request */
    if (!t)
        p = process_get(0);
    else
        p = t->parent;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD || !p->files[fd].used)
        return -EINVAL;

    request.inode = p->files[fd].inode;
    request.size = count;
    request.off = p->files[fd].offset;

    if (p->files[fd].dev >= 0)
        ret = device_read_write(p, p->files[fd].dev, &request, buf, VFS_READ);
    else
    {
        if (!p->files[fd].mount->ops->read)
            return -ENOSYS;

        ret = p->files[fd].mount->ops->read(p->files[fd].mount, p, &request,
                                            buf);
    }

    if (ret < 0)
        return ret;

    p->files[fd].offset = request.off;

    return ret;
}
