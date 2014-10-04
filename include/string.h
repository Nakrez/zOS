#ifndef STRING_H
# define STRING_H

# include <kernel/types.h>

# ifndef NULL
#  define NULL ((void *)0)
# endif /* !NULL */

void *memcpy(void *dest, const void *src, size_t size);
void *memset(void *ptr, int value, size_t size);

size_t strlen(const char *str);
char *strcpy(char *destination, const char *source);

int strcmp(const char *str1, const char *str2);
char *strtok_r(char *str, const char *delim, char **saveptr);

char *strchr(const char *s, int c);
char *strcat(char *dest, const char *src);

#endif /* !STRING_H */
