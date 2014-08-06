#ifndef X86_CPU_H
# define X86_CPU_H

# include <arch/crx.h>
# include <arch/regs.h>

static inline void cpu_flush_tlb(void)
{
    __asm__ __volatile__ ("mov %%cr3, %%eax\n"
                          "mov %%eax, %%cr3\n"
                          :
                          :
                          : "memory");
}

#endif /* !X86_CPU_H */
