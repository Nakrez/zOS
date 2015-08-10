#ifndef LIBC_FCNTL_H
# define LIBC_FCNTL_H

# define O_RDONLY (1 << 0)
# define O_WRONLY (1 << 1)
# define O_RDWR (O_RDONLY | O_WRONLY)
# define O_APPEND (1 << 2)

int open(const char *pathname, int flags, int mode);

#endif /* !LIBC_FCNTL_H */
