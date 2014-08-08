#include <kernel/thread.h>
#include <kernel/kmalloc.h>

int thread_create(struct process *process, uintptr_t code)
{
    struct thread *thread;

    if (!code)
        return 0;

    thread = kmalloc(sizeof (struct thread));

    if (!thread)
        return 0;

    thread->uid = 0;
    thread->gid = 0;

    if (!_thread.create(process, thread, code))
    {
        kfree(thread);
        return 0;
    }

    klist_add(&process->threads, &thread->list);

    /* FIXME: Add thread to scheduler */

    return 1;
}
