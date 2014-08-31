#ifndef AS_H
# define AS_H

# include <kernel/klist.h>

# include <glue/as.h>

# include <arch/spinlock.h>

# define AS_MAP_USER 1
# define AS_MAP_WRITE (1 << 2)
# define AS_MAP_EXEC (1 << 3)

/* Release the physical page when unmap */
# define AS_UNMAP_RELEASE 1

/* Don't release physical page when unmap */
# define AS_UNMAP_NORELEASE 0

struct as_mapping
{
    vaddr_t virt;
    struct segment *phy;
    size_t size;
    int flags;

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
    int (*unmap)(struct as *, vaddr_t, size_t);
    int (*duplicate)(struct as *, struct as *);
    paddr_t (*virt_to_phy)(vaddr_t);
    int (*destroy)(struct as *);
};

extern struct as_glue as_glue_dispatcher;

extern struct as kernel_as;

/*
 * Creates a new address space and returns it
 */
struct as *as_create(void);

/*
 * Initializes a new address space
 */
int as_initialize(struct as* as);

/*
 * Get physical address mapped on virtual address
 */
static inline paddr_t as_virt_to_phy(vaddr_t vaddr)
{
    return (paddr_t)glue_call(as, virt_to_phy, vaddr);
}

struct as_mapping *as_mapping_locate(struct as *as, vaddr_t vaddr);

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
 * Duplicate an entire address space relying on COW technique
 */
struct as *as_duplicate(struct as *as);

/*
 * Check if a buffer is mapped for size
 */
int as_is_mapped(struct as *as, vaddr_t ptr, size_t size);

/*
 * Remap an existing mapping with different flags
 */
int as_remap(struct as *as, struct as_mapping *map, int flags);

/*
 * Copy buffers between 2 address spaces
 */
int as_copy(struct as *src_as, struct as *dest_as, void *src, void *dest,
            size_t size);
/*
 * Unmap vaddr
 */
void as_unmap(struct as *as, vaddr_t vaddr, int flags);

void as_destroy(struct as *as);

#endif /* !AS_H */
