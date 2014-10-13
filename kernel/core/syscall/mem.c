#include <kernel/errno.h>
#include <kernel/syscall.h>

#include <kernel/mem/as.h>

#include <kernel/proc/thread.h>

#include <arch/mmu.h>

# define MMAP_PROT_WRITE (1 << 2)
# define MMAP_PROT_EXEC (1 << 3)

struct mmap_args {
    void *addr;
    size_t length;
    int prot;
    int flags;
    int fd;
    int offset;
};

int sys_mmap(struct syscall *interface)
{
    struct mmap_args *args = (void *)interface->arg1;
    vaddr_t res;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) args,
                      sizeof (struct mmap_args)))
        return -EFAULT;

    /* Make sure rigths are correct */
    args->prot = (args->prot & (MMAP_PROT_WRITE | MMAP_PROT_EXEC));

    /* MMAP flags are the same as AS, so we can combine them */
    args->prot |= AS_MAP_USER;

    if ((uintptr_t)args->addr > KERNEL_BEGIN ||
        (uintptr_t)args->addr + args->length > KERNEL_BEGIN)
        return -EINVAL;

    if (!args->length)
        return -EINVAL;

    res = as_map(thread_current()->parent->as, (vaddr_t)args->addr, 0,
                 args->length, args->prot);

    if (!res)
        return -ENOMEM;

    return res;
}

int sys_munmap(struct syscall *interface)
{
    void *addr = (void *)interface->arg1;
    int length = interface->arg2;

    if ((uintptr_t)addr > KERNEL_BEGIN ||
        (uintptr_t)addr + length > KERNEL_BEGIN)
        return -EINVAL;

    as_unmap(thread_current()->parent->as, (vaddr_t)addr, AS_UNMAP_RELEASE);

    return 0;
}
