#include <string.h>

#include <kernel/errno.h>
#include <kernel/console.h>
#include <kernel/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/tmpfs.h>
#include <kernel/vfs/vops.h>
#include <kernel/vfs/vdevice.h>

#include <kernel/kmalloc.h>

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

dev_t vfs_device_create(const char *name, pid_t pid, int perm, int ops)
{
    int res;
    struct vdevice *device = NULL;
    char *node_path;

    res = device_create(pid, name, ops, &device);

    if (res < 0)
        return res;

    /*
     * If mknod fails we don't really care because the device exists node the
     * node in the FS, but it can be created later
     */

    /* 5 = strlen("/dev/") */
    if (!(node_path = kmalloc(5 + strlen(name) + 1)))
        return device->id;

    strcpy(node_path, "/dev/");

    strcat(node_path, name);

    vfs_mknod(thread_current(), node_path, perm, device->id);

    return device->id;
}
