#include <string.h>
#include <sys/mman.h>

#include <arch/syscall.h>

struct mmap_args {
    void *addr;
    size_t length;
    int prot;
    int flags;
    int fd;
    int offset;
};

void *mmap(void *addr, size_t length, int prot, int flags, int fd,
           off_t offset)
{
    struct mmap_args args;
    void *test = &args;
    int ret;

    args.addr = addr;
    args.length = length;
    args.prot = prot;
    args.flags = flags;
    args.fd = fd;
    args.offset = offset;

    SYSCALL1(SYS_MMAP, test, ret);

    if (ret < 0)
        return NULL;

    return (void *)ret;
}
