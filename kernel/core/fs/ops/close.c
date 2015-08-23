#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/device.h>

int vfs_close(struct thread *t, int fd)
{
    int ret;
    struct process *p;
    struct file *file;

    /* Kernel request */
    if (!t)
        p = process_get(0);
    else
        p = thread_current()->parent;

    ret = process_file_from_fd(p, fd, &file);
    if (ret < 0)
        return ret;

    if (!file->f_ops->close) {
        process_free_fd(p, fd);
        return 0;
    }

    ret = file->f_ops->close(file, file->inode->inode);
    if (ret < 0)
        return ret;

    process_free_fd(p, fd);

    return 0;
}
