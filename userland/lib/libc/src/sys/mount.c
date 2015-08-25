#include <sys/mount.h>

#include <arch/syscall.h>

int mount(const char *fs_name, const char *device, const char *mount_pt)
{
    int ret;

    SYSCALL3(SYS_MOUNT, fs_name, device, mount_pt, ret);

    return ret;
}
