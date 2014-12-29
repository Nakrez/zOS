#ifndef LIBC_STRING_H
# define LIBC_STRING_H

# include <stdint.h>

# ifndef NULL
#  define NULL ((void *)0)
# endif /* !NULL */

size_t strlen(const char *s);

char *strcpy(char *dest, const char *src);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);

void *memcpy(void *dest, const void *src, size_t size);
void *memset(void *ptr, int value, size_t num);
int memcmp(const void *s1, const void *s2, size_t n);

char *strtok_r(char *str, const char *delim, char **saveptr);

#endif /* !LIBC_STRING_H */
