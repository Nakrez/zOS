#ifndef LIBC_UNISTD_H
# define LIBC_UNISTD_H

# include <stdint.h>

int usleep(size_t mseconds);
int sleep(size_t seconds);
int fork(void);

#endif /* !LIBC_UNISTD_H */
