#include <unistd.h>
#include <sys/stat.h>

#include <arch/syscall.h>

int stat(const char *path, struct stat *buf)
{
    int ret;

    SYSCALL2(SYS_STAT, path, buf, ret);

    return ret;
}
