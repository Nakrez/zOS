#include <arch/gdt.h>
#include <arch/pm.h>

#include <string.h>

#include <kernel/kmalloc.h>
#include <kernel/console.h>

struct gdt_entry *gdt_entries;

void gdt_add_entry(int num, uint32_t base, uint32_t limit, uint8_t access,
                   uint8_t granularity)
{
    struct gdt_entry *entry = &gdt_entries[num];

    entry->limitl = limit & 0xFFFF;
    entry->limith = (limit >> 16) & 0xF;
    entry->basel = base & 0xFFFFFF;
    entry->baseh = (base >> 24) & 0xFF;
    entry->flags = granularity << 2;
    entry->access = access;
}

void gdt_init(void)
{
    struct gdt_ptr gptr;
    gdt_entries = kmalloc(sizeof (struct gdt_entry) * GDT_MAX_SIZE);

    /* NULL segment */
    memset(gdt_entries, 0, sizeof (struct gdt_entry));

    gdt_add_entry(GDT_KERNEL_CS, 0, 0xFFFFF, CS(0), 3);
    gdt_add_entry(GDT_KERNEL_DS, 0, 0xFFFFF, DS(0), 3);
    gdt_add_entry(GDT_USER_CS, 0, 0xFFFFF, CS(3), 3);
    gdt_add_entry(GDT_USER_DS, 0, 0xFFFFF, DS(3), 3);

    gptr.size = GDT_MAX_SIZE * sizeof (struct gdt_entry) - 1;
    gptr.ptr = (uintptr_t)gdt_entries;

    __asm__ __volatile__ ("lgdtl %0"
                          :
                          : "m" (gptr)
                          : "memory");

    console_message(T_OK, "GDT initialized");
}
