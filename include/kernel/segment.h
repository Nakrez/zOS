#ifndef SEGMENT_H
# define SEGMENT_H

# include <boot/boot.h>

# include <kernel/types.h>
# include <kernel/klist.h>

struct segment_glue
{
    void (*init)(void);
};

struct segment
{
    /* Start address */
    paddr_t base;

    /* Size in page (for real size multiply by PAGE_SIZE) */
    uint32_t page_size;

    /* Can be used ? */
    int free;

    struct klist list;
};

extern struct segment_glue __segment;

/*
 * Initialize segments (physical memory) layout
 * It call glue init member to initialize low level architecture dependent
 * physical memory initialization
 */

void segment_initialize(struct boot_info *boot);

/*
 * Allocate page_size pages of physical memory
 *
 * Return the physical address of the segment or 0 if it fails
 */
paddr_t segment_alloc(uint32_t page_size);

/*
 * Reserve physical memory starting at addr and with size (* PAGE_SIZE)
 * page_size
 *
 * Return 1 if it worked, 0 otherwise
 */
int segment_reserve(paddr_t addr, uint32_t page_size);

/*
 * Free the segment starting at addr
 */
void segment_free(paddr_t addr);

/*
 * Dump segment free and used
 */
void segment_dump(void);

#endif /* !SEGMENT_H */
