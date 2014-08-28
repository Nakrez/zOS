#include <string.h>

#include <kernel/vfs/path_tree.h>
#include <kernel/vfs/vfs.h>
#include <kernel/kmalloc.h>
#include <kernel/errno.h>

static struct vtree_node *__root = NULL;

static struct vtree_node *vtree_node_create(struct vnode *node)
{
    struct vtree_node *tnode;

    if (!(tnode = kmalloc(sizeof (struct vtree_node))))
        return NULL;

    tnode->vnode = node;

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

    klist_add(&__root->sons, &dev_tree->brothers);

    return 0;

error:
    kfree(root);
    kfree(dev);
    kfree(root_tree);
    kfree(dev_tree);
    return -1;
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

    return size - 1;
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
                cur = tmp;

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

                    found = 1;
                    break;
                }

            }
        }

        spinlock_unlock(&cur->sons_lock);

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
