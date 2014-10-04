#include <string.h>

#include <kernel/errno.h>
#include <kernel/console.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/tmpfs.h>
#include <kernel/vfs/vops.h>
#include <kernel/vfs/path_tree.h>
#include <kernel/vfs/vdevice.h>
#include <kernel/vfs/vnode.h>

#include <kernel/kmalloc.h>

int vfs_initialize(void)
{
    if (vtree_initialize())
        return -1;

    if (vfs_mount("/", TMPFS_DEV_ID) < 0)
    {
        console_message(T_ERR, "Fail to mount tmpfs on /");

        return -1;
    }

    console_message(T_OK, "Tmpfs mounted on /");

    if (vfs_mkdir("/dev", 0, 0, 0755) < 0)
    {
        console_message(T_ERR, "Fail to create /dev");

        return -1;
    }

    if (vfs_mount("/dev", TMPFS_DEV_ID) < 0)
    {
        console_message(T_ERR, "Fail to mount tmpfs on /dev");

        return -1;
    }

    console_message(T_OK, "Tmpfs mounted on /dev");

    return 0;
}

int vfs_device_create(const char *name, int pid, int uid, int gid, int perm,
                      int ops)
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

    vfs_mknod(node_path, uid, gid, perm, device->id);

    return device->id;
}
