#ifndef LIBC_UNISTD_H
# define LIBC_UNISTD_H

# include <stdint.h>

# ifndef NULL
#  define NULL ((void *)0)
# endif /* !NULL */

# define SEEK_SET (1 << 0)
# define SEEK_CUR (1 << 1)
# define SEEK_END (1 << 2)

int usleep(size_t mseconds);
int sleep(size_t seconds);
int fork(void);
pid_t getpid(void);
pid_t gettid(void);

int execv(const char *filename, char *const argv[]);

int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int close(int fd);
int lseek(int fd, off_t off, int whence);

struct stat;

int stat(const char *path, struct stat *buf);
int fstat(int fd, struct stat *buf);

#endif /* !LIBC_UNISTD_H */
