#ifndef GLUE_I386_PC_CPU_H
# define GLUE_I386_PC_CPU_H

# include <arch/tss.h>

struct cpu;

struct cpu_glue_data
{
    struct tss tss;
};

void i386_pc_cpu_initialize(struct cpu *cpu);

#endif /* !GLUE_I386_PC_CPU_H */
