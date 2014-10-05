#include <string.h>

#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/fs.h>

int vfs_lseek(int fd, int offset, int whence)
{
    struct process *process = thread_current()->parent;

    if (!(whence & VFS_SEEK_SET) && !(whence && VFS_SEEK_CUR) &&
        !(whence && VFS_SEEK_END))
        return -EINVAL;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD)
        return -EINVAL;

    if (!process->files[fd].used)
        return -EINVAL;

    if (whence & VFS_SEEK_SET)
    {
        if (offset < 0)
            return -EINVAL;

        process->files[fd].offset = offset;
    }
    else if (whence & VFS_SEEK_CUR)
    {
        size_t old_off = process->files[fd].offset;

        process->files[fd].offset += offset;

        if (offset < 0 && process->files[fd].offset > old_off)
        {
            process->files[fd].offset = old_off;

            return -EINVAL;
        }

        if (offset > 0 && process->files[fd].offset < old_off)
        {
            process->files[fd].offset = old_off;

            return -EINVAL;
        }
    }
    else
        kernel_panic("VFS_SEEK_END not implemented yet");

    return 0;
}
