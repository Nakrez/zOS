#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/vfs/tmpfs.h>

void *tmpfs_initialize(void)
{
    struct tmpfs_root *root = kmalloc(sizeof (struct tmpfs_root));

    if (!root)
        return NULL;

    root->inode_table = kmalloc(sizeof (struct tmpfs_node *) *
                                TMPFS_INODE_TABLE_INIT_SIZE);

    if (!root->inode_table)
    {
        kfree(root);

        return NULL;
    }

    root->inode_table_size = TMPFS_INODE_TABLE_INIT_SIZE;
    root->inode_current = 1;

    klist_head_init(&root->root_sons);

    memset(root->inode_table, 0, sizeof (struct tmpfs_node *) *
           TMPFS_INODE_TABLE_INIT_SIZE);

    return root;
}

static int tmpfs_lookup(struct mount_entry *root, const char *path,
                        uid_t uid, gid_t gid, struct resp_lookup *ret)
{
    (void)uid;
    (void)gid;

    int found = 0;
    char *part;
    char *path_left;
    char *path_copy = kmalloc(strlen(path) + 1);
    struct tmpfs_root *sb = root->private;
    struct klist *node_list = &sb->root_sons;
    struct tmpfs_node *node = NULL;
    struct tmpfs_node *tmp;

    if (!path_copy)
        return -ENOMEM;

    ret->processed = 0;

    if (*path == '/')
        ret->processed = 1;

    if (klist_empty(node_list))
    {
        ret->inode = 0;
        ret->dev = -1;
        ret->ret = RES_KO;

        kfree(path_copy);

        return 0;
    }

    if (*path == '/')
        strcpy(path_copy, path + 1);
    else
        strcpy(path_copy, path);

    part = strtok_r(path_copy, "/", &path_left);

    while (part)
    {
        klist_for_each_elem(node_list, tmp, brothers)
        {
            if (!strcmp(part, tmp->name))
            {
                node_list = &tmp->sons;
                node = tmp;

                found = 1;

                break;
            }
        }

        if (!found)
        {
            if (!node)
            {
                ret->inode = 0;
                ret->dev = -1;
                ret->ret = RES_KO;
            }
            else
            {
                ret->inode = node->inode;
                ret->dev = node->dev;
                ret->ret = RES_KO;
            }


            return 0;
        }

        found = 0;

        ret->processed += path_left - part;

        if (node->type == TMPFS_TYPE_MOUNT)
        {
            ret->ret = RES_ENTER_MOUNT;
            ret->dev = node->dev;
            ret->inode = node->inode;

            return 0;
        }

        part = strtok_r(NULL, "/", &path_left);
    }

    ret->inode = node->inode;
    ret->dev = node->dev;
    ret->ret = RES_OK;

    return 0;
}

static int tmpfs_mkdir(struct mount_entry *root, const char *path, ino_t inode,
                       uid_t uid, gid_t gid, mode_t mode)
{
    struct tmpfs_root *sb = root->private;
    struct tmpfs_node *new_node;

    if (!*path)
        return -EEXIST;

    if (strchr(path, '/'))
        return -ENOENT;

    if (!(new_node = kmalloc(sizeof (struct tmpfs_node))))
        return -ENOMEM;

    if (!(new_node->name = kmalloc(strlen(path) + 1)))
    {
        kfree(new_node);

        return -ENOMEM;
    }

    strcpy(new_node->name, path);

    new_node->inode = sb->inode_current++;
    new_node->type = TMPFS_TYPE_DIR;
    new_node->uid = uid;
    new_node->gid = gid;
    new_node->perm = mode;
    new_node->dev = -1;
    new_node->content = NULL;

    klist_head_init(&new_node->sons);

    sb->inode_table[new_node->inode] = new_node;

    if (inode == 0)
        klist_add(&sb->root_sons, &new_node->brothers);
    else
        klist_add(&sb->inode_table[inode]->sons, &new_node->brothers);

    return 0;
}

static int tmpfs_mknod(struct mount_entry *root, const char *path, ino_t inode,
                       uid_t uid, gid_t gid, mode_t mode, dev_t dev)
{
    struct tmpfs_root *sb = root->private;
    struct tmpfs_node *new_node;

    if (!*path)
        return -EEXIST;

    if (strchr(path, '/'))
        return -ENOENT;

    if (!(new_node = kmalloc(sizeof (struct tmpfs_node))))
        return -ENOMEM;

    if (!(new_node->name = kmalloc(strlen(path) + 1)))
    {
        kfree(new_node);

        return -ENOMEM;
    }

    strcpy(new_node->name, path);

    new_node->inode = sb->inode_current++;
    new_node->type = TMPFS_TYPE_DEV;
    new_node->uid = uid;
    new_node->gid = gid;
    new_node->perm = mode;
    new_node->dev = dev;
    new_node->content = NULL;

    klist_head_init(&new_node->sons);

    sb->inode_table[new_node->inode] = new_node;

    if (inode == 0)
        klist_add(&sb->root_sons, &new_node->brothers);
    else
        klist_add(&sb->inode_table[inode]->sons, &new_node->brothers);
    return 0;
}

static int tmpfs_mount(struct mount_entry *root, ino_t inode, int mount_pt_nb)
{
    struct tmpfs_root *sb = root->private;

    if (sb->inode_table[inode]->type != TMPFS_TYPE_DIR)
        return -ENOTDIR;

    /* TODO: Check busy */
    sb->inode_table[inode]->dev = mount_pt_nb;
    sb->inode_table[inode]->type = TMPFS_TYPE_MOUNT;

    return 0;
}

static int tmpfs_open(struct mount_entry *root, ino_t inode, uid_t uid,
                      gid_t gid, int flags, mode_t mode)
{
    (void)root;
    (void)inode;
    (void)uid;
    (void)gid;
    (void)flags;
    (void)mode;

    return 0;
}

static void tmpfs_cleanup(void *data)
{
    (void)data;
}

struct fs_ops tmpfs_ops = {
    .init = tmpfs_initialize,
    .lookup = tmpfs_lookup,
    .mkdir = tmpfs_mkdir,
    .mknod = tmpfs_mknod,
    .mount = tmpfs_mount,
    .open = tmpfs_open,
    .cleanup = tmpfs_cleanup,
};
