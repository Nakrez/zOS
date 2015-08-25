#ifndef FS_VFS_MOUNT_H
# define FS_VFS_MOUNT_H

# include <kernel/types.h>

struct fs;

struct mount_entry {
    int used;

    char *path;

    struct fs_instance *fi;

    /* Private data, only used by kernel file system */
    void *private;
};

struct thread;

int vfs_mount(struct thread *t, const char *fs_name, const char *device,
              const char *mount_pt);
struct mount_entry *vfs_root_get(void);
struct mount_entry *vfs_mount_pt_get(const char *path);

#endif /* !FS_VFS_MOUNT_H */
