#include <arch/thread.h>

struct thread_glue _thread =
{
    i386_thread_create,
};
