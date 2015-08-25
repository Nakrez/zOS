#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/device.h>
#include <kernel/fs/vfs/message.h>

int vfs_read(struct thread *t, int fd, void *buf, size_t count)
{
    int ret;
    struct process *p;
    struct req_rdwr req;
    struct file *file;

    /* Kernel request */
    if (!t)
        p = process_get(0);
    else
        p = t->parent;

    ret = process_file_from_fd(p, fd, &file);
    if (ret < 0)
        return ret;

    if (!file->f_ops->read)
        return -ENOSYS;

    req.inode = 0;
    if (file->inode)
        req.inode = file->inode->inode;

    req.size = count;
    req.off = file->offset;

    ret = file->f_ops->read(file, p, &req, buf);
    if (ret < 0)
        return ret;

    file->offset = req.off;

    return ret;
}
