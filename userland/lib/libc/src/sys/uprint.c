#include <zos/print.h>

#include <arch/syscall.h>

void uprint(const char *s)
{
    int ret;

    SYSCALL1(SYS_UPRINT, s, ret);
}
