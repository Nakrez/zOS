#ifndef VFS_H
# define VFS_H

# define VFS_MAX_FILENAME_SIZE 255

# define VFS_OPS_OPEN (1 << 0)
# define VFS_OPS_READ (1 << 1)
# define VFS_OPS_WRITE (1 << 2)
# define VFS_OPS_CLOSE (1 << 3)
# define VFS_OPS_UMOUNT (1 << 4)

int vfs_initialize(void);
int vfs_device_create(const char *name, int pid, int uid, int gid, int perm,
                      int ops);

#endif /* !VFS_H */
