#ifndef X86_CRX_H
# define X86_CRX_H

# include <kernel/types.h>

# define CR0_PAGE 0x80000000
# define CR0_PM 0x1

# define CR4_BIGPAGE 0x00000010

static inline uint32_t cr0_get(void)
{
    uint32_t cr0;

    __asm__ __volatile__("mov %%cr0, %%eax\n"
                         "mov %%eax, %0"
                         : "=r" (cr0)
                         :);

    return cr0;
}

static inline void cr0_set(uint32_t cr0)
{
    __asm__ __volatile__("mov %0, %%eax\n"
                         "mov %%eax, %%cr0"
                         :
                         : "r" (cr0));
}

static inline uint32_t cr3_get(void)
{
    uint32_t cr3;

    __asm__ __volatile__("mov %%cr3, %%eax\n"
                         "mov %%eax, %0"
                         : "=r" (cr3)
                         :);

    return cr3;
}

static inline void cr3_set(uint32_t cr3)
{
    __asm__ __volatile__("mov %0, %%eax\n"
                         "mov %%eax, %%cr3"
                         :
                         : "r" (cr3));
}

static inline uint32_t cr4_get(void)
{
    uint32_t cr4;

    __asm__ __volatile__("mov %%cr4, %%eax\n"
                         "mov %%eax, %0"
                         : "=r" (cr4)
                         :);

    return cr4;
}

static inline void cr4_set(uint32_t cr4)
{
    __asm__ __volatile__("mov %0, %%eax\n"
                         "mov %%eax, %%cr4"
                         :
                         : "r" (cr4));
}

#endif /* !X86_CRX_H */
