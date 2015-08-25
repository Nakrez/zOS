#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/device.h>

int vfs_ioctl(struct thread *t, int fd, int request, int *argp)
{
    int ret;
    struct process *p;
    struct req_ioctl req;
    struct file *file;

    /* Kernel request */
    if (!t)
        p = process_get(0);
    else
        p = t->parent;

    ret = process_file_from_fd(p, fd, &file);
    if (ret < 0)
        return ret;

    if (!file->f_ops->ioctl)
        return -ENOSYS;

    req.inode = 0;
    if (file->inode)
        req.inode = file->inode->inode;

    req.request = request;
    req.with_argp = argp != NULL;

    if (argp)
        req.argp = *argp;

    return file->f_ops->ioctl(file, &req, argp);
}
