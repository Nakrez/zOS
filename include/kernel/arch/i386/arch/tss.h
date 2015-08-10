#ifndef ARCH_I386_TSS_H
# define ARCH_I386_TSS_H

# include <kernel/types.h>

struct cpu;

struct tss
{
    uint16_t previous_task, __previous_task;
    uint32_t esp0;
    uint16_t ss0, __ss0;
    uint32_t esp1;
    uint16_t ss1, __ss1;
    uint32_t esp2;
    uint16_t ss2, __ss2;
    uint32_t cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t es, __es;
    uint16_t cs, __cs;
    uint16_t ss, __ss;
    uint16_t ds, __ds;
    uint16_t fs, __fs;
    uint16_t gs, __gs;
    uint16_t ldt, __ldt;
    uint16_t debug, io;
} __attribute__ ((packed));

void tss_initialize(struct cpu *cpu);

#endif /* !ARCH_I386_TSS_H */
