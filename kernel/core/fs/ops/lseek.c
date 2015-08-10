#include <string.h>

#include <kernel/errno.h>
#include <kernel/panic.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>

int vfs_lseek(struct thread *t, int fd, off_t offset, int whence)
{
    struct file *file;
    struct process *process;

    /* Kernel request */
    if (!t)
        process = process_get(0);
    else
        process = t->parent;

    if (!(whence & VFS_SEEK_SET) && !(whence & VFS_SEEK_CUR) &&
        !(whence & VFS_SEEK_END))
        return -EINVAL;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD)
        return -EINVAL;

    file = &process->files[fd];

    if (!file->used)
        return -EINVAL;

    if (whence & VFS_SEEK_SET) {
        if (offset < 0)
            return -EINVAL;

        file->offset = offset;
    } else if (whence & VFS_SEEK_CUR) {
        size_t old_off = file->offset;

        file->offset += offset;

        if (offset < 0 && file->offset > old_off) {
            file->offset = old_off;
            return -EINVAL;
        }

        if (offset > 0 && file->offset < old_off) {
            process->files[fd].offset = old_off;
            return -EINVAL;
        }
    } else {
        kernel_panic("VFS_SEEK_END not implemented yet");
    }

    return 0;
}
