#ifndef VFS_FS_H
# define VFS_FS_H

# include <kernel/types.h>

# include <kernel/proc/process.h>

# include <kernel/vfs/mount.h>
# include <kernel/vfs/message.h>
# include <kernel/vfs/vops.h>

struct fs_ops {
    void *(*init)(void);
    int (*lookup)(struct mount_entry *, const char *, uid_t, gid_t,
                  struct resp_lookup *);
    int (*mkdir)(struct mount_entry *, const char *, ino_t, uid_t, gid_t,
                 mode_t);
    int (*mknod)(struct mount_entry *, const char *, ino_t, uid_t, gid_t,
                 mode_t, dev_t);
    int (*stat)(struct mount_entry *, uid_t, gid_t, ino_t, struct stat *);
    int (*mount)(struct mount_entry *, ino_t, int);
    int (*open)(struct mount_entry *, ino_t, pid_t, uid_t, gid_t, int, mode_t);
    int (*read)(struct mount_entry *, struct process *, struct req_rdwr *,
                void *buf);
    int (*write)(struct mount_entry *, struct process *, struct req_rdwr *,
                 const void *buf);
    int (*close)(struct mount_entry *, ino_t);
    void (*cleanup)(void *);
};

extern struct fs_ops fiu_ops;

#endif /* !VFS_FS_H */
