#include <kernel/timer.h>

void timer_initialize(void)
{
    __timer.init();
}
