#ifndef LIBC_STRING_H
# define LIBC_STRING_H

# include <stdint.h>

# ifndef NULL
#  define NULL ((void *)0)
# endif /* !NULL */

size_t strlen(const char *s);

char *strcpy(char *dest, const char *src);
void *memcpy(void *dest, const void *src, size_t size);

#endif /* !LIBC_STRING_H */
