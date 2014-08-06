#include <boot/boot.h>

#include <kernel/console.h>
#include <kernel/kmalloc.h>
#include <kernel/segment.h>
#include <kernel/as.h>
#include <kernel/panic.h>

int kernel_main(struct boot_info *boot)
{
    (void)boot;

    console_init();

    console_message(T_INF, "zKernel is booting");

    kmalloc_initialize(boot);

    segment_initialize(boot);

    console_message(T_OK, "Initialization of kernel address space");

    if (!as_initialize(&kernel_as))
        kernel_panic("Fail to initialize kernel address space");

    while (1)
        ;
}
