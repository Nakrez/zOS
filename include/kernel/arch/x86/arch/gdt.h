#ifndef ARCH_X86_GDT_H
# define ARCH_X86_GDT_H

# include <kernel/types.h>

# define GDT_MAX_SIZE 5

# define GDT_KERNEL_CS 1
# define GDT_KERNEL_DS 2
# define GDT_USER_CS 3
# define GDT_USER_DS 4

# define SELECTOR(entry, dpl) ((entry << 3) + dpl)
# define KERNEL_SELECTOR(entry) SELECTOR(entry, 0)
# define USER_SELECTOR(entry) SELECTOR(entry, 3)

# define SEGMENT_PRESENT 0x80

# define CS(dpl) (SEGMENT_PRESENT | (dpl << 5) | (1 << 4) | 0xA)
# define DS(dpl) (SEGMENT_PRESENT | (dpl << 5) | (1 << 4) | 0x2)

struct gdt_ptr
{
    uint16_t size;
    uint32_t ptr;
} __attribute__ ((packed));

struct gdt_entry
{
    uint16_t limitl;
    uint32_t basel : 24;
    uint32_t access : 8;
    uint8_t limith : 4;
    uint8_t flags : 4;
    uint8_t baseh;
} __attribute__ ((packed));

void gdt_init(void);
void gdt_add_entry(int num, uint32_t base, uint32_t limit, uint8_t access,
                   uint8_t granularity);

#endif /* !ARCH_X86_GDT_H */
