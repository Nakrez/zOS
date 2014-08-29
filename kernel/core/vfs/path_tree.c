#include <string.h>

#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>

#include <kernel/vfs/path_tree.h>
#include <kernel/vfs/vfs.h>

static struct vtree_node *__root = NULL;

/* Global lock for tree modification */
static spinlock_t vtree_lock = SPINLOCK_INIT;

static struct vtree_node *vtree_node_create(struct vnode *node)
{
    struct vtree_node *tnode;

    if (!(tnode = kmalloc(sizeof (struct vtree_node))))
        return NULL;

    tnode->vnode = node;
    tnode->father = NULL;

    spinlock_init(&tnode->sons_lock);

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

    dev_tree->father = __root;

    klist_add(&__root->sons, &dev_tree->brothers);

    return 0;

error:
    kfree(root);
    kfree(dev);
    kfree(root_tree);
    kfree(dev_tree);
    return -1;
}

int vtree_insert(const char *path, struct vnode *vnode)
{
    int res;
    struct vtree_node *parent;
    struct vtree_node *vtree_node;
    const char *remaining;

    spinlock_lock(&vtree_lock);

    res = vtree_lookup(path, &remaining, &parent);

    if (res < 0)
    {
        spinlock_unlock(&vtree_lock);
        return res;
    }

    /* FIXME: Panic, not sure what to do here, fix when you know :) */

    /* Will the path always be populated on insert ? */
    if (*remaining)
        kernel_panic("vtree_insert: Path not fully populated...");

    /*
     * If the path is fully populated there should not be any fake
     * directories. Not sure that this flag wil be used anyway
     */
    if (parent->vnode->type & VFS_TYPE_FAKE)
        kernel_panic("vtree_insert: Fake directory encountered...");

    if (!(parent->vnode->type & VFS_TYPE_DIR))
    {
        spinlock_unlock(&vtree_lock);
        return -ENOENT;
    }

    vtree_node = vtree_node_create(vnode);

    if (!vtree_node)
    {
        spinlock_unlock(&vtree_lock);
        return -ENOMEM;
    }

    vtree_node->father = parent;

    spinlock_lock(&parent->sons_lock);

    klist_add(&parent->sons, &vtree_node->brothers);

    spinlock_unlock(&parent->sons_lock);

    spinlock_unlock(&vtree_lock);

    return 0;
}

static int vtree_next_lookup(const char **remaining_path, char *result)
{
    const char *path = *remaining_path;

    int size = 0;

    while (*path && *path != '/')
    {
        if (size >= VFS_MAX_FILENAME_SIZE)
            return -ENAMETOOLONG;

        result[size++] = *(path++);
    }

    result[size] = 0;

    /* Skip / */
    while (*path == '/')
        ++path;

    *remaining_path = path;

    return size;
}

static int path_cmp(const char *str1, const char *str2, int size)
{
    while (size--)
    {
        if (*str1 == 0 || *str1 == '/')
            return -1;

        if (*str2 == 0 || *str2 == '/')
            return -1;

        if (*(str1++) != *(str2++))
            return -1;
    }

    return !((*str1 == 0 || *str1 == '/') &&
             (*str2 == 0 || *str2 == '/'));
}

int vtree_lookup(const char *path, const char **remaining_path,
                 struct vtree_node **node)
{
    int res;
    int found;

    /* Store current lookup name */
    char current[VFS_MAX_FILENAME_SIZE + 1];

    /* Best node dialog for current node */
    struct vtree_node *res_node = __root;
    struct vtree_node *cur = __root;
    struct vtree_node *tmp;

    const char *res_path = path;

    *remaining_path = path;

    while (**remaining_path == '/')
        ++(*remaining_path);

    while (**remaining_path)
    {
        /*
         * If there is still path to explore and our last node was a file
         * then we can't find it
         */
        if (cur->vnode->type & VFS_TYPE_FILE)
            return -ENOENT;

        found = 0;

        res = vtree_next_lookup(remaining_path, current);

        if (res < 0)
            return res;

        spinlock_lock(&cur->sons_lock);

        klist_for_each_elem(&cur->sons, tmp, brothers)
        {
            if (path_cmp(tmp->vnode->name, current, res) == 0)
            {
                found = 1;

                /*
                 * If we fond a device or a virtual directory, change
                 * the node responsible for the remaining path
                 */
                if ((tmp->vnode->type & VFS_TYPE_DEVICE) ||
                    ((tmp->vnode->type & VFS_TYPE_VIRTUAL &&
                      tmp->vnode->type & VFS_TYPE_DIR)))
                {
                    res_node = tmp;
                    res_path = *remaining_path;

                    break;
                }
            }
        }

        spinlock_unlock(&cur->sons_lock);

        cur = tmp;

        if (found)
            continue;

        /* We did not find the element. If the root node is virtual that means
         * that with have a virtual directory so we did not found it.
         * Otherwise, we return the root node supposed to contain the file
         * and the path remaining, so that the VFS will be able to contact
         * the responsible driver to try to locate the path
         */

        if ((res_node->vnode->type & VFS_TYPE_VIRTUAL) &&
            (res_node->vnode->type & VFS_TYPE_DIR))
            return -ENOENT;

        *remaining_path = res_path;
        *node = res_node;

        return 0;
    }

    /* We found the total path. cur holds the real vnode */
    *node = cur;

    return 0;
}
