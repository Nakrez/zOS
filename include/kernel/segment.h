#ifndef SEGMENT_H
# define SEGMENT_H

# include <boot/boot.h>

# include <kernel/types.h>
# include <kernel/klist.h>

# define SEGMENT_FLAGS_NONE 0
# define SEGMENT_FLAGS_COW 1

struct segment_glue
{
    int (*init)(void);
};

struct segment
{
    /* Start address */
    paddr_t base;

    /* Size in page (for real size multiply by PAGE_SIZE) */
    uint32_t page_size;

    /* Can be used ? */
    uint8_t free : 1;

    uint8_t flags : 1;

    struct klist list;
};

extern struct segment_glue segment_glue_dispatcher;

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
struct segment *segment_alloc(uint32_t page_size);

paddr_t segment_alloc_address(uint32_t page_size);

/*
 * Reserve physical memory starting at addr and with size (* PAGE_SIZE)
 * page_size
 *
 * Return 1 if it worked, 0 otherwise
 */
int segment_reserve(paddr_t addr, uint32_t page_size);

struct segment *segment_locate(paddr_t addr);

void segment_release(struct segment *seg);

/*
 * Free the segment starting at addr
 */
void segment_free(paddr_t addr);

/*
 * Dump segment free and used
 */
void segment_dump(void);

#endif /* !SEGMENT_H */
