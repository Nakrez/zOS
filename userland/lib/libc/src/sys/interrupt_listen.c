#include <zos/interrupt.h>

#include <arch/syscall.h>

int interrupt_listen(int interrupt)
{
    int ret;

    SYSCALL1(SYS_INTERRUPT_LISTEN, interrupt, ret);

    return ret;
}
