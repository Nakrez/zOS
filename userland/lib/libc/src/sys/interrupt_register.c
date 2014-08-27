#include <zos/interrupt.h>

#include <arch/syscall.h>

int interrupt_register(int interrupt)
{
    int ret;

    SYSCALL1(SYS_INTERRUPT_REGISTER, interrupt, ret);

    return ret;
}
