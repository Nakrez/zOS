#ifndef KMALLOC_H
# define KMALLOC_H

#include <boot/boot.h>

# define KSTACK_SIZE 0x1000

void kmalloc_initialize(struct boot_info *boot);

#endif /* !KMALLOC_H */
