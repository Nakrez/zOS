#ifndef I386_CPU_H
# define I386_CPU_H

# include <kernel/types.h>

# include <arch/crx.h>
# include <arch/regs.h>

struct thread_regs
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t __esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};

struct irq_regs
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t irq_num;
    uint32_t irq_data;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t user_ss;
};

static inline void cpu_invalid_page(void *address)
{
    __asm__ __volatile__("invlpg (%0)\n"
                         :
                         : "r" (address)
                         : "memory");
}

static inline void cpu_flush_tlb(void)
{
    __asm__ __volatile__ ("mov %%cr3, %%eax\n"
                          "mov %%eax, %%cr3\n"
                          :
                          :
                          : "memory", "eax");
}

static inline void cpu_irq_enable(void)
{
    __asm__ __volatile__ ("sti\n");
}

static inline void cpu_irq_disable(void)
{
    __asm__ __volatile__ ("cli\n");
}

#endif /* !I386_CPU_H */
