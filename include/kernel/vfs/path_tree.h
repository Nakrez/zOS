#ifndef VFS_PATH_TREE_H
# define VFS_PATH_TREE_H

# include <kernel/klist.h>
# include <kernel/vfs/vnode.h>

# include <arch/spinlock.h>

struct vtree_node {
    struct vnode *vnode;

    spinlock_t sons_lock;

    struct klist sons;

    struct klist brothers;
};

int vtree_initialize(void);
int vtree_lookup(const char *path, const char **remaining_path,
                 struct vtree_node **node);

#endif /* !VFS_PATH_TREE_H */
