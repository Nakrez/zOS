int kernel_main(void)
{
    volatile char *screen = (char *)0xC00B8000;

    *screen = 'K';
    *(screen + 1) = 0xF;

    while (1)
        ;
}
