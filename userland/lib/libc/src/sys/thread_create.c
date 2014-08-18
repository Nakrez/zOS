#include <thread.h>

#include <arch/syscall.h>

static void thread_entry(void (*callback)(void *), void *arg)
{
    callback(arg);

    thread_exit();
}

int thread_create(void (*callback)(void *), void *arg)
{
    int ret;

    SYSCALL3(SYS_THREAD_CREATE, thread_entry, callback, arg, ret);

    return ret;
}
