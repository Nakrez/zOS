#ifndef AS_H
# define AS_H

# include <kernel/klist.h>

# include <glue/as.h>

# include <arch/spinlock.h>

# define AS_MAP_USER 1
# define AS_MAP_WRITE (1 << 2)
# define AS_MAP_EXEC (1 << 3)

# define AS_UNMAP_RELEASE 1
# define AS_UNMAP_NORELEASE 0

struct as_mapping
{
    vaddr_t virt;
    paddr_t phy;
    size_t size;

    struct klist list;
};

struct as
{
    spinlock_t region_lock;
    struct klist regions;

    spinlock_t map_lock;
    struct klist mapping;

    struct glue_as arch;
};

struct as_glue
{
    int (*init)(struct as *);
    int (*map)(struct as *, vaddr_t, paddr_t, size_t, int);
    void (*unmap)(struct as *, vaddr_t, size_t);
};

extern struct as kernel_as;
extern struct as_glue __as;

/*
 * Creates a new address space and returns it
 */
struct as *as_create(void);

/*
 * Initializes a new address space
 */
int as_initialize(struct as* as);

/*
 * Map paddr to vaddr for a size size
 *
 * If vaddr == 0 the function will try to locate a region of size size
 * if paddr == 0 the function will try to locate a segment of size size
 *
 * Return vaddr if everything went well 0 otherwise
 */
vaddr_t as_map(struct as *as, vaddr_t vaddr, paddr_t paddr, size_t size,
               int flags);

/*
 * Unmap vaddr
 */
void as_unmap(struct as *as, vaddr_t vaddr, int flags);

#endif /* !AS_H */
