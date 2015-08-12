#include <zos/vfs.h>

#include <arch/syscall.h>

int fs_unregister(const char *fs_name)
{
    int ret;

    SYSCALL1(SYS_FS_UNREGISTER, fs_name, ret);

    return ret;
}
