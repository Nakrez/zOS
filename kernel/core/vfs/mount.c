#include <string.h>

#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/vdevice.h>
#include <kernel/vfs/path_tree.h>

#include <arch/spinlock.h>

# define MAX_MOUNTED_PATH 1

struct mount_entry {
    int used;

    const char *path;
    struct vnode *node;
};

static struct mount_entry mount_points[MAX_MOUNTED_PATH];
static spinlock_t mount_lock = SPINLOCK_INIT;

int vfs_mount(int fd, const char *mount_path)
{
    int res;
    int mount_nb = -1;
    const char *remaining;
    struct vdevice *device;
    struct vnode *node;
    struct vtree_node *tnode;

    if (!(device = device_get(fd)))
        return -EBADF;

    node = device->node;

    if (!(node->type & VFS_TYPE_FS))
        return -EINVAL;

    if (!(node->type & VFS_TYPE_DIR))
        return -ENOTDIR;

    spinlock_lock(&mount_lock);

    for (int i = 0; i < MAX_MOUNTED_PATH; ++i)
    {
        if (!mount_points[i].used)
        {
            if (mount_nb == -1)
                mount_nb = i;

            continue;
        }

        if (node == mount_points[i].node)
            return -EBUSY;

        if (!strcmp(mount_points[i].path, mount_path))
            return -EBUSY;
    }

    if (mount_nb == -1)
    {
        spinlock_unlock(&mount_lock);

        return -ENOMEM;
    }

    mount_points[mount_nb].used = 1;

    spinlock_unlock(&mount_lock);

    res = vtree_lookup(mount_path, &remaining, &tnode);

    if (res < 0)
    {
        mount_points[mount_nb].used = 0;

        return res;
    }

    if (!klist_empty(&tnode->sons))
    {
        if (strcmp(tnode->vnode->name, "/")
            || !(tnode->vnode->type & VFS_TYPE_FAKE))
        {
            mount_points[mount_nb].used = 0;

            return -EBUSY;
        }
    }

    ++node->ref_count;

    vnode_destroy(tnode->vnode);

    tnode->vnode = node;

    /* TODO: mount_point[mount_nb] */

    return 0;
}
