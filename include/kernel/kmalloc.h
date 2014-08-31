#ifndef KMALLOC_H
# define KMALLOC_H

#include <boot/boot.h>

# define KSTACK_SIZE 0x1000

void kmalloc_initialize(struct boot_info *boot);
void *kmalloc(size_t size);
void kfree(void *ptr);
void kmalloc_dump(void);

#endif /* !KMALLOC_H */
