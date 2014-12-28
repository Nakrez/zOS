#ifndef LIBC_DIRENT_H
# define LIBC_DIRENT_H

# include <sys/types.h>

# define NAME_MAX 256

# define DIR_OK 1
# define DIR_END 0

struct dirent {
    ino_t d_ino;

    char d_name[NAME_MAX];
};

typedef struct {
    int fd;

    struct dirent dp;

    int index;
} DIR;

int getdirent(int fd, struct dirent *d, int index);

#endif /* !LIBC_DIRENT_H */
