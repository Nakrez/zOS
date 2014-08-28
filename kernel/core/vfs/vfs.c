#include <kernel/errno.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/path_tree.h>
#include <kernel/vfs/vdevice.h>
#include <kernel/vfs/vnode.h>

#include <kernel/kmalloc.h>

int vfs_initialize(void)
{
    if (vtree_initialize())
        return -1;

    return 0;
}

int vfs_device_create(const char *name, int pid, int uid, int gid, int perm)
{
    int res;
    struct vdevice *device = NULL;
    struct vnode *node = NULL;

    res = device_create(pid, name, &device);

    if (res < 0)
        return res;

    node = vnode_create(name, uid, gid, perm,
                        VFS_TYPE_CHARDEV | VFS_TYPE_FILE);

    if (!node)
    {
        res = -ENOMEM;

        goto error;
    }

    node->dev = device->id;

    res = vtree_insert("/dev", node);

    if (res < 0)
        goto error;

    return device->id;

error:
    vnode_destroy(node);
    device_destroy(pid, device->id);

    return res;
}
