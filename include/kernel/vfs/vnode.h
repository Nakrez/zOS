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

# define VFS_DEV_MAJOR_NONE 0x0
# define VFS_DEV_MINOR_NONE 0x0

# define MAKEDEV(major, minor) (((major & 0xFFFF) << 16) | (minor & 0xFFFF))
# define NODEV MAKEDEV(VFS_DEV_MAJOR_NONE, VFS_DEV_MINOR_NONE)

struct vnode {
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
