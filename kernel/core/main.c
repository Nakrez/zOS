#include <boot/boot.h>

#include <kernel/console.h>

int kernel_main(struct boot_info *boot)
{
    (void)boot;

    console_init();

    console_message(T_INF, "zKernel is booting");

    while (1)
        ;
}
