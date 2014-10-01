#ifndef VFS_TMPFS_H
# define VFS_TMPFS_H

# include <kernel/klist.h>
# include <kernel/types.h>

# include <kernel/vfs/fs.h>

# define TMPFS_DEV_ID -1

# define TMPFS_INODE_TABLE_INIT_SIZE 20

# define TMPFS_TYPE_FILE 1
# define TMPFS_TYPE_DIR 2
# define TMPFS_TYPE_DEV 3
# define TMPFS_TYPE_MOUNT 4

struct tmpfs_node {
    char *name;

    ino_t inode;

    uint8_t type;

    uint16_t uid;

    uint16_t gid;

    mode_t perm;

    uint16_t dev;

    void *content;

    struct klist brothers;

    struct klist sons;
};

struct tmpfs_root {
    struct tmpfs_node **inode_table;

    size_t inode_table_size;

    size_t inode_current;

    struct klist root_sons;
};

extern struct fs_ops tmpfs_ops;

#endif /* !VFS_TMPFS_H */
