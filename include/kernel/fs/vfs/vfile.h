#ifndef FS_VFS_VFILE_H
# define FS_VFS_VFILE_H

# include <kernel/types.h>

# include <kernel/fs/vfs/mount.h>

# define VFS_MODE_UNUSED 0

# define VFS_MODE_R (1 << 1)
# define VFS_MODE_W (1 << 2)

/* Special mode for fd being used by the driver to get messages */
# define VFS_MODE_CHANNEL (1 << 3)

struct vfile {
    int used;

    char mode;

    /* Current offset in the file */
    off_t offset;

    ino_t inode;

    dev_t dev;

    struct mount_entry *mount;
};

#endif /* !FS_VFS_VFILE_H */
