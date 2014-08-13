#include <glue/segment.h>

#include <arch/gdt.h>
#include <arch/pm.h>

struct segment_glue segment_glue_dispatcher =
{
    segment_x86_pc_initialize,
};

int segment_x86_pc_initialize(void)
{
    gdt_init();
    pm_setup();

    return 1;
}
