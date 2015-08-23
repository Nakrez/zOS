#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/device.h>
#include <kernel/fs/vfs/message.h>

int vfs_write(struct thread *t, int fd, const void *buf, size_t count)
{
    int ret;
    struct process *p;
    struct req_rdwr request;
    struct file *file;

    /* Kernel request */
    if (!t)
        p = process_get(0);
    else
        p = t->parent;

    ret = process_file_from_fd(p, fd, &file);
    if (ret < 0)
        return ret;

    if (!file->f_ops->write)
        return -ENOSYS;

    request.inode = file->inode->inode;
    request.size = count;
    request.off = file->offset;

    ret = file->f_ops->write(file, p, &request, (void *)buf);
    if (ret < 0)
        return ret;

    file->offset = request.off;

    return ret;
}
