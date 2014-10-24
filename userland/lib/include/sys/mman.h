#ifndef LIBC_I386_SYS_MMAN_H
# define LIBC_I386_SYS_MMAN_H

# include <stdint.h>

# define PROT_WRITE (1 << 2)
# define PROT_EXEC (1 << 3)

void *mmap(void *addr, size_t length, int prot, int flags, int fd,
           off_t offset);
int munmap(void *addr, size_t length);

#endif /* !LIBC_I386_SYS_MMAN_H */
