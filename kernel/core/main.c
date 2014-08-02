#include <boot/boot.h>

#include <kernel/console.h>

int kernel_main(struct boot_info *boot)
{
    (void)boot;

    console_init();

    console_puts("zKernel is booting ...\n");

    while (1)
        ;
}
