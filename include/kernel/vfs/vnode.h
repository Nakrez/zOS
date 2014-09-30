#ifndef VFS_VNODE_H
# define VFS_VNODE_H

# include <kernel/types.h>

/* Virtual node permissions */
# define VFS_PERM_USER_R 0400
# define VFS_PERM_USER_W 0200
# define VFS_PERM_USER_E 0100

# define VFS_PERM_GROUP_R 040
# define VFS_PERM_GROUP_W 020
# define VFS_PERM_GROUP_E 010

# define VFS_PERM_OTHER_R 04
# define VFS_PERM_OTHER_W 02
# define VFS_PERM_OTHER_E 01

# define VFS_PERM_STICKY 01000
# define VFS_PERM_SGID 02000
# define VFS_PERM_SUID 04000

/* Virtual node type */
# define VFS_TYPE_NONE (1 << 0)
# define VFS_TYPE_CHARDEV (1 << 1)
# define VFS_TYPE_BLOCKDEV (1 << 2)
# define VFS_TYPE_DIR (1 << 3)
# define VFS_TYPE_FILE (1 << 4)
# define VFS_TYPE_VIRTUAL (1 << 5)
# define VFS_TYPE_FAKE (1 << 6)
# define VFS_TYPE_FS (1 << 7)

# define VFS_TYPE_DEVICE (VFS_TYPE_CHARDEV | VFS_TYPE_BLOCKDEV)

# define VFS_DEVICE_NONE 0

struct vnode {
    /*
     * If this is a file or directory belonging to a fs it index helps indicate
     * which file is targeted by a request
     */
    int index;

    /* Name of the node */
    char *name;

    /* User id and group id */
    int uid;
    int gid;

    /* Node permission */
    uint16_t perm;

    /* Node type */
    int type;

    /* Device identification (to transfer request to driver) */
    uint32_t dev;

    /* Reference count on the node (useful for deallocation) */
    int ref_count;

    /* Useful for intermediate nodes */
    struct vnode *parent;
};

struct vnode *vnode_create(const char *name, int uid, int gid, uint16_t perm,
                           int type);

void vnode_destroy(struct vnode *node);

#endif /* !VFS_VNODE_H */
