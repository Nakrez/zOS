#ifndef VFS_VOPS_H
# define VFS_VOPS_H

# include <kernel/types.h>

# include <kernel/vfs/message.h>

# define VFS_SEEK_SET (1 << 0)
# define VFS_SEEK_CUR (1 << 1)
# define VFS_SEEK_END (1 << 2)

struct mount_entry;

int vfs_lookup(const char *path, int uid, int gid, struct resp_lookup *res,
               struct mount_entry **mount_pt);
int vfs_mkdir(const char *path, int uid, int gid, mode_t mode);
int vfs_mknod(const char *path, int uid, int gid, mode_t mode, uint16_t dev);
int vfs_open(const char *path, int uid, int gid, int flags, mode_t mode);
int vfs_read(int fd, void *buf, size_t count);
int vfs_close(int fd);
int vfs_lseek(int fd, int offset, int whence);

int vfs_write(int fd, const void *buf, size_t count);

#endif /* !VFS_VOPS_H */
