#include <kernel/cpu.h>

#include <glue/cpu.h>

#include <arch/tss.h>

struct cpu_glue cpu_glue_dispatcher =
{
    i386_pc_cpu_initialize,
};

int i386_pc_cpu_initialize(struct cpu *cpu)
{
    tss_initialize(cpu);

    return 1;
}
