#ifndef VFS_H
# define VFS_H

# define VFS_MAX_FILENAME_SIZE 255

# define VFS_OPEN 1
# define VFS_READ 2
# define VFS_WRITE 3
# define VFS_CLOSE 4
# define VFS_UMOUNT 5
# define VFS_MOUNT 6
# define VFS_LOOKUP 7
# define VFS_MKDIR 8
# define VFS_MKNOD 9
# define VFS_STAT 10
# define VFS_IOCTL 11

# define VFS_OPS_OPEN (1 << 0)
# define VFS_OPS_READ (1 << 1)
# define VFS_OPS_WRITE (1 << 2)
# define VFS_OPS_CLOSE (1 << 3)
# define VFS_OPS_UMOUNT (1 << 4)
# define VFS_OPS_MOUNT (1 << 5)
# define VFS_OPS_LOOKUP (1 << 6)
# define VFS_OPS_MKDIR (1 << 7)
# define VFS_OPS_MKNOD (1 << 8)
# define VFS_OPS_STAT (1 << 9)
# define VFS_OPS_IOCTL (1 << 10)

int vfs_initialize(void);
dev_t vfs_device_create(const char *name, pid_t pid, int perm, int ops);

#endif /* !VFS_H */
