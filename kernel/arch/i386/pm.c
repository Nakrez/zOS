#include <arch/pm.h>
#include <arch/cpu.h>

#include <kernel/console.h>

void pm_setup(void)
{
    cr0_set(cr0_get() | CR0_PM);

    cs_set(KERNEL_CS);
    ds_set(KERNEL_DS);
    es_set(KERNEL_DS);
    fs_set(KERNEL_DS);
    gs_set(KERNEL_DS);
    ss_set(KERNEL_DS);

    console_message(T_OK, "Kernel now running in protected-mode");
}
