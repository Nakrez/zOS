#ifndef LIBC_UNISTD_H
# define LIBC_UNISTD_H

# include <stdint.h>

typedef int pid_t;

int usleep(size_t mseconds);
int sleep(size_t seconds);
int fork(void);
pid_t getpid(void);
pid_t gettid(void);

int read(int fd, void *buf, size_t count);

#endif /* !LIBC_UNISTD_H */
