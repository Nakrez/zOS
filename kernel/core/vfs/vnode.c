#include <string.h>

#include <kernel/vfs/vnode.h>
#include <kernel/zos.h>
#include <kernel/kmalloc.h>

struct vnode *vnode_create(const char *name, int uid, int gid, uint16_t perm,
                           int type)
{
    struct vnode *node;

    if (!(node = kmalloc(sizeof (struct vnode))))
        return NULL;

    if (!(node->name = kmalloc(strlen(name) + 1)))
    {
        kfree(node);
        return NULL;
    }

    strcpy(node->name, name);

    node->uid = uid;
    node->gid = gid;
    node->perm = perm;
    node->type = type;
    node->dev = VFS_DEVICE_NONE;
    node->parent = NULL;

    return node;
}

void vnode_destroy(struct vnode *node)
{
    if (!node)
        return;

    kfree(node->name);
    kfree(node);
}
