#ifndef VFS_PATH_TREE_H
# define VFS_PATH_TREE_H

# include <kernel/klist.h>
# include <kernel/vfs/vnode.h>

struct vtree_node {
    struct vnode *vnode;

    struct klist sons;

    struct klist brothers;
};

int vtree_initialize(void);

#endif /* !VFS_PATH_TREE_H */
