#include <thread.h>
#include <stdarg.h>
#include <stdlib.h>

#include <arch/syscall.h>

static void thread_entry(int argc, void *argv[])
{
    ((thread_callback_t)argv[0])(argc - 1, argv + 1);

    thread_exit();
}

int thread_create(thread_callback_t entry, int argc, ...)
{
    int ret;
    int i = 0;
    va_list ap;
    void **argv = malloc(sizeof (void *) * (argc + 2));

    if (!argv)
        return -1;

    va_start(ap, argc);

    argv[0] = entry;

    for (; i < argc; ++i)
        argv[i + 1] = va_arg(ap, void *);

    va_end(ap);

    argv[i + 1] = NULL;

    SYSCALL2(SYS_THREAD_CREATE, thread_entry, argv, ret);

    free(argv);

    return ret;
}
