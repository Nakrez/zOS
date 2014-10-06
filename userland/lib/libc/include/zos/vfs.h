#ifndef ZOS_VFS_H
# define ZOS_VFS_H

# include <stdint.h>
# include <sys/types.h>

# define VFS_OPS_OPEN (1 << 0)
# define VFS_OPS_READ (1 << 1)
# define VFS_OPS_WRITE (1 << 2)
# define VFS_OPS_CLOSE (1 << 3)
# define VFS_OPS_UMOUNT (1 << 4)
# define VFS_OPS_MOUNT (1 << 5)
# define VFS_OPS_LOOKUP (1 << 6)
# define VFS_OPS_MKDIR (1 << 7)
# define VFS_OPS_MKNOD (1 << 8)

/*
 * Special request emitted by the kernel when remounting root with userland
 * file system driver
 */
struct req_root_remount {
    int mount_pt;

    char path[100];
};

/* Lookup request */
struct req_lookup {
    char *path;
    uint16_t path_size;
    uint16_t uid;
    uint16_t gid;
};

struct resp_lookup {
    int ret;
    ino_t inode;
    int processed;
    int dev;
};

/* Open request */
struct req_open {
    ino_t inode;
    uint16_t uid;
    uint16_t gid;
    int flags;
    int mode;
};

/* Open response */
struct resp_open {
    int ret;
    ino_t inode;
};

/* Read/Write request */
struct req_rdwr {
    ino_t inode;

    size_t size;

    size_t off;

    void *data;
};

/* Read/Write response */
struct resp_rdwr {
    int ret;

    size_t size;
};

/* Close request */
struct req_close {
    ino_t inode;
};

/* Close response */
struct resp_close {
    int ret;
};

#endif /* !ZOS_VFS_H */
