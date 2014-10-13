#ifndef VFS_MOUNT_H
# define VFS_MOUNT_H

struct mount_entry {
    int used;

    char *path;
    struct vnode *node;

    int dev;

    struct fs_ops *ops;

    /* Private data, only used by kernel file system */
    void *private;
};

struct thread;

int vfs_mount(struct thread *t, int dev, const char *mount_path);
struct mount_entry *vfs_root_get(void);
struct mount_entry *vfs_mount_pt_get(const char *path);

#endif /* !VFS_MOUNT_H */
