#include <kernel/cpu.h>

#include <arch/tss.h>
#include <arch/gdt.h>
#include <arch/pm.h>

void tss_initialize(struct cpu *cpu)
{
    int tss_sel = TSS_SELECTOR(cpu->id);

    uint32_t tss_base = (uint32_t)&cpu->arch.tss;
    uint32_t tss_size = tss_base + sizeof (struct tss);

    gdt_add_entry(GDT_TSS_BASE + cpu->id, tss_base, tss_size, TSS(3), 0);

    cpu->arch.tss.ss0 = KERNEL_DS;

    __asm__ __volatile__ ("mov %0, %%ax\n"
                          "ltr %%ax\n"
                          :
                          : "g" (tss_sel));
}
