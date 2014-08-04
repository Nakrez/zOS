#include <boot/boot.h>

#include <kernel/console.h>
#include <kernel/kmalloc.h>

int kernel_main(struct boot_info *boot)
{
    (void)boot;

    console_init();

    console_message(T_INF, "zKernel is booting");

    kmalloc_initialize(boot);

    while (1)
        ;
}
