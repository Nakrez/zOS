#ifndef FS_VFS_MOUNT_H
# define FS_VFS_MOUNT_H

# include <kernel/types.h>

struct mount_entry {
    int used;

    char *path;
    struct vnode *node;

    dev_t dev;

    struct fs_operation *fs_ops;

    struct file_operation *f_ops;

    /* Private data, only used by kernel file system */
    void *private;
};

struct thread;

int vfs_mount(struct thread *t, dev_t dev, const char *mount_path);
struct mount_entry *vfs_root_get(void);
struct mount_entry *vfs_mount_pt_get(const char *path);

#endif /* !FS_VFS_MOUNT_H */
