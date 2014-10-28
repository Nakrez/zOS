#include <sys/mman.h>

#include <arch/syscall.h>

void *mmap_physical(void *addr, size_t length)
{
    void *ret;

    SYSCALL2(SYS_MMAP_PHYSICAL, addr, length, ret);

    return ret;
}
