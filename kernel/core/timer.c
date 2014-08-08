#include <kernel/timer.h>

void timer_initialize(void)
{
    __timer.init();
}

void timer_handler(int irq, int data)
{
    (void) irq;
    (void) data;
}
