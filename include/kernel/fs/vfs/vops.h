#ifndef FS_VFS_VOPS_H
# define FS_VFS_VOPS_H

# include <kernel/types.h>

# define VFS_SEEK_SET (1 << 0)
# define VFS_SEEK_CUR (1 << 1)
# define VFS_SEEK_END (1 << 2)

# define NAME_MAX 256

struct resp_lookup;
struct mount_entry;
struct thread;

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};

struct dirent {
    ino_t d_ino;

    char d_name[NAME_MAX];
};

int vfs_lookup(struct thread *t, const char *path, struct resp_lookup *res,
               struct mount_entry **mount_pt);
int vfs_mkdir(struct thread *t, const char *path, mode_t mode);
int vfs_mknod(struct thread *t, const char *path, mode_t mode, uint16_t dev);
int vfs_stat(struct thread *t, const char *path, struct stat *buf);
int vfs_fstat(struct thread *t, int fd, struct stat *buf);
int vfs_open(struct thread *t, const char *path, int flags, mode_t mode);
int vfs_open_device(struct thread *t, const char *device_name, int flags,
                    mode_t mode);
int vfs_read(struct thread *t, int fd, void *buf, size_t count);
int vfs_write(struct thread *t, int fd, const void *buf, size_t count);
int vfs_close(struct thread *t, int fd);
int vfs_lseek(struct thread *t, int fd, off_t offset, int whence);
int vfs_ioctl(struct thread *t, int fd, int req, int *argp);
int vfs_dup(struct thread *t, int oldfd);
int vfs_dup2(struct thread *t, int oldfd, int newfd);
int vfs_getdirent(struct thread *t, int fd, struct dirent *dirent, int index);

#endif /* !FS_VFS_VOPS_H */
