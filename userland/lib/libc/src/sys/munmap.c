#include <sys/mman.h>

#include <arch/syscall.h>

int munmap(void *addr, size_t length)
{
    int ret;

    SYSCALL2(SYS_MUNMAP, addr, length, ret);

    return ret;
}
