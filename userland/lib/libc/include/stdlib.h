#ifndef LIBC_STDLIB_H
# define LIBC_STDLIB_H

# include <stdint.h>

# ifndef NULL
#  define NULL ((void *)0)
# endif /* !NULL */

void exit(int status);

void *malloc(size_t size);
void free(void *ptr);

#endif /* !LIBC_STDLIB_H */
