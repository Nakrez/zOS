#ifndef SEGMENT_H
# define SEGMENT_H

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
    uint64_t page_size;

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

void segment_dump(void);

#endif /* !SEGMENT_H */
