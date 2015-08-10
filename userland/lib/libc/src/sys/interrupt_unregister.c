#include <zos/interrupt.h>

#include <arch/syscall.h>

void interrupt_unregister(int interrupt)
{
    int ret;

    SYSCALL1(SYS_INTERRUPT_UNREGISTER, interrupt, ret);
}
