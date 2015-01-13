#ifndef LIBC_STDLIB_H
# define LIBC_STDLIB_H

# include <stdint.h>

# ifndef NULL
#  define NULL ((void *)0)
# endif /* !NULL */

void exit(int status);

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

char *getenv(const char *name);

long int strtol(const char *nptr, char **endptr, int base);

#endif /* !LIBC_STDLIB_H */
