#include <zos/interrupt.h>

#include <arch/syscall.h>

int interrupt_listen(void)
{
    int ret;

    SYSCALL0(SYS_INTERRUPT_LISTEN, ret);

    return ret;
}
