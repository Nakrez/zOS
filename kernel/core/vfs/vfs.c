#include <string.h>

#include <kernel/errno.h>
#include <kernel/console.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/tmpfs.h>
#include <kernel/vfs/vops.h>
#include <kernel/vfs/vdevice.h>

int vfs_initialize(void)
{
    if (vfs_mount(NULL, TMPFS_DEV_ID, "/") < 0)
    {
        console_message(T_ERR, "Fail to mount tmpfs on /");

        return -1;
    }

    console_message(T_OK, "Tmpfs mounted on /");

    if (vfs_mkdir(NULL, "/dev", 0755) < 0)
    {
        console_message(T_ERR, "Fail to create /dev");

        return -1;
    }

    if (vfs_mount(NULL, TMPFS_DEV_ID, "/dev") < 0)
    {
        console_message(T_ERR, "Fail to mount tmpfs on /dev");

        return -1;
    }

    console_message(T_OK, "Tmpfs mounted on /dev");

    return 0;
}
