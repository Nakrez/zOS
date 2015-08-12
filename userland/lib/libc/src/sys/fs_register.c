#include <zos/vfs.h>

#include <arch/syscall.h>

int fs_register(const char *fs_name, int channel_fd, vop_t ops)
{
    int ret;

    SYSCALL3(SYS_FS_REGISTER, fs_name, channel_fd, ops, ret);

    return ret;
}
