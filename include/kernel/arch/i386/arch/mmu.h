#ifndef I386_MMU_H
# define I386_MMU_H

# include <kernel/zos.h>

# define PD_PRESENT (1 << 0)
# define PD_WRITE (1 << 1)
# define PD_USER (1 << 2)
# define PD_4MB (1 << 7)

# define PT_PRESENT PD_PRESENT
# define PT_WRITE PD_WRITE
# define PT_USER PD_USER

# define PAGE_SIZE 0x1000

/* USER BEGIN IS 0x1000 so 0x0 is never mapped */
# define USER_BEGIN 0x1000

# define KERNEL_BEGIN 0xC0000000
# define KERNEL_END 0xFFFFFFFF

# define KERNEL_PHY_PD 0x1000
# define KERNEL_VIRT_PD 0xC0001000

# define KERNEL_EXTRA_PT_BEGIN 0x2000
# define KERNEL_EXTRA_PT_END 0x80000
# define KERNEL_EXTRA_PT_COUNT ((KERNEL_EXTRA_PT_END - \
                                 KERNEL_EXTRA_PT_BEGIN) / PAGE_SIZE)

/*
 * Kernel will always be able to work between 0xC0000000 and 0xC0400000
 * Extendable heap is extra allocatable memory for the kernel
 */
# define KERNEL_EXHEAP_START 0xC0400000
# define KERNEL_EXHEAP_END (KERNEL_BEGIN + KERNEL_EXTRA_PT_COUNT * 4 * MB)

# define KERNEL_EXHEAP_SIZE (KERNEL_EXHEAP_END - KERNEL_EXHEAP_START)

struct as;

int mmu_init_kernel(struct as *as);
int mmu_init_user(struct as *as);
paddr_t mmu_virt_to_phy(vaddr_t vaddr);
int mmu_map(struct as *as, vaddr_t vaddr, paddr_t paddr, size_t size,
            int flags);
int mmu_unmap(struct as *as, vaddr_t vaddr, size_t size);
int mmu_duplicate(struct as *old, struct as *new);
void mmu_remove_cr3(struct as *as);

#endif /* !I386_MMU_H */
