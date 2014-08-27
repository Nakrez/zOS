#include <kernel/vfs/path_tree.h>
#include <kernel/kmalloc.h>

static struct vtree_node *__root = NULL;

static struct vtree_node *vtree_node_create(struct vnode *node)
{
    struct vtree_node *tnode;

    if (!(tnode = kmalloc(sizeof (struct vtree_node))))
        return NULL;

    tnode->vnode = node;

    klist_head_init(&tnode->sons);

    return tnode;
}

int vtree_initialize(void)
{
    /* We create the following tree
     * /
     *   dev
     */
    struct vnode *root = NULL;
    struct vnode *dev = NULL;

    struct vtree_node *root_tree = NULL;
    struct vtree_node *dev_tree = NULL;

    if (!(root = vnode_create("/", 0, 0, 0755, VFS_TYPE_DIR | VFS_TYPE_FAKE)))
        goto error;

    if (!(dev = vnode_create("dev", 0, 0, 0755,
                             VFS_TYPE_DIR | VFS_TYPE_VIRTUAL)))
        goto error;

    if (!(root_tree = vtree_node_create(root)))
        goto error;

    if (!(dev_tree = vtree_node_create(dev)))
        goto error;

    __root = root_tree;

    klist_add(&__root->sons, &dev_tree->brothers);

    return 0;

error:
    kfree(root);
    kfree(dev);
    kfree(root_tree);
    kfree(dev_tree);
    return -1;
}
