#ifndef STRING_H
# define STRING_H

# include <kernel/types.h>

void *memcpy(void *dest, const void *src, size_t size);
void *memset(void *ptr, int value, size_t size);

size_t strlen(const char *str);
char *strcpy(char *destination, const char *source);

#endif /* !STRING_H */
