#include <arch/thread.h>

struct thread_glue thread_glue_dispatcher =
{
    i386_thread_create,
    i386_thread_current,
};
