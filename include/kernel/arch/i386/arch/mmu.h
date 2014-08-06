#ifndef I386_MMU_H
# define I386_MMU_H

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

struct as;

int mmu_init_kernel(struct as *as);
int mmu_init_user(struct as *as);

#endif /* !I386_MMU_H */
