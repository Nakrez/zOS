#ifndef VFS_VOPS_H
# define VFS_VOPS_H

int vfs_open(const char *pathname, int flags, int mode);
int vfs_read(int fd, void *buf, size_t count);
int vfs_write(int fd, const void *buf, size_t count);

#endif /* !VFS_VOPS_H */
