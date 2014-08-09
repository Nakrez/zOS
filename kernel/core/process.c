#include <kernel/process.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>
#include <kernel/thread.h>

static struct klist processes;

void process_initialize(void)
{
    klist_head_init(&processes);
}

static int process_new_pid(void)
{
    struct process *process;
    int used;
    int pid = 0;

    for (; pid < PROCESS_MAX_PID; ++pid)
    {
        used = 0;

        klist_for_each_elem(&processes, process, list)
        {
            if (process->pid == pid)
                used = 1;
        }

        if (!used)
            return pid;
    }

    return -1;
}

struct process *process_create(int type, uintptr_t code, int flags)
{
    struct process *process;
    int pid;

    pid = process_new_pid();

    if (pid < 0)
        return NULL;

    process = kmalloc(sizeof (struct process));

    if (!process)
        return NULL;

    if (type & PROCESS_TYPE_USER)
    {
        process->as = kmalloc(sizeof (struct as));

        if (!process || !as_initialize(process->as))
            goto error;
    }
    else
        process->as = &kernel_as;

    process->thread_count = 0;
    process->type = type;

    /* Init thread list */
    klist_head_init(&process->threads);

    if (flags & PROCESS_FLAG_LOAD)
        kernel_panic("Loading binary not implemented");

    if (!thread_create(process, code))
        goto error;

    klist_add(&processes, &process->list);

    return process;

error:
    if (process->as != &kernel_as)
        kfree(process->as);

    return NULL;
}
