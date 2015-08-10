#ifndef REGION_H
# define REGION_H

# include <kernel/types.h>
# include <kernel/klist.h>

# include <kernel/mem/as.h>

struct region
{
    vaddr_t base;

    uint32_t page_size;

    int mapped;

    struct klist list;
};

/*
 * Initialize region inside an address space
 */
void region_initialize(struct as *as);

/*
 * Reserve a region inside an address space
 *
 * if addr == 0 it just try to locate a region with size = page_size
 *
 * Return the address at the beginning of the region, 0 if it fails
 */
vaddr_t region_reserve(struct as *as, vaddr_t addr, size_t page_size);

/*
 * Release a region inside an address space
 */
void region_release(struct as *as, vaddr_t addr);

/*
 * Dump region in a specified address space
 */
void region_dump(struct as *as);

#endif /* !REGION_H */
