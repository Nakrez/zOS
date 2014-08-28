#ifndef VFS_H
# define VFS_H

# define VFS_MAX_FILENAME_SIZE 255

int vfs_initialize(void);
int vfs_device_create(const char *name, int pid, int uid, int gid, int perm);

#endif /* !VFS_H */
