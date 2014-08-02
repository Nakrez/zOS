#include <boot/boot.h>

int kernel_main(struct boot_info *boot)
{
    (void)boot;

    volatile char *screen = (char *)0xC00B8000;

    *screen = 'K';
    *(screen + 1) = 0xF;

    while (1)
        ;
}
