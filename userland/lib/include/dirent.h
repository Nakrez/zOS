#ifndef LIBC_DIRENT_H
# define LIBC_DIRENT_H

# include <sys/types.h>

# define NAME_MAX 256

struct dirent {
    ino_t d_ino;

    char d_name[NAME_MAX];
};

typedef struct {
    int fd;

    struct dirent dp;

    int index;
} DIR;

#endif /* !LIBC_DIRENT_H */
