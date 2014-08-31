#ifndef UTILS_H
# define UTILS_H

# include <kernel/types.h>

void *memcpy(void *dest, const void *src, size_t size);
void *memset(void *ptr, int value, size_t size);

#endif /* !UTILS_H */
