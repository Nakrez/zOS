#include <glue/thread.h>

struct thread_glue _thread =
{
    i386_pc_thread_create,
};

int i386_pc_thread_create(struct process *p, struct thread *t, uintptr_t eip)
{
    return 1;
}
