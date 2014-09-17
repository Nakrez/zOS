#ifndef VFS_VOPS_H
# define VFS_VOPS_H

# define VFS_SEEK_SET (1 << 0)
# define VFS_SEEK_CUR (1 << 1)
# define VFS_SEEK_END (1 << 2)

int vfs_open(const char *pathname, int flags, int mode);
int vfs_read(int fd, void *buf, size_t count);
int vfs_write(int fd, const void *buf, size_t count);
int vfs_close(int fd);
int vfs_lseek(int fd, int offset, int whence);

#endif /* !VFS_VOPS_H */
