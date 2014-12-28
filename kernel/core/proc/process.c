#include <string.h>

#include <kernel/zos.h>
#include <kernel/errno.h>
#include <kernel/panic.h>
#include <kernel/cpu.h>

#include <kernel/mem/segment.h>
#include <kernel/mem/region.h>
#include <kernel/mem/as.h>
#include <kernel/mem/kmalloc.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>
#include <kernel/proc/elf.h>

static struct klist processes;

void process_initialize(void)
{
    klist_head_init(&processes);
}

static void init_process(struct process *p, pid_t pid, int type,
                         struct process *parent)
{
    p->state = PROCESS_STATE_ALIVE;
    p->thread_count = 0;
    p->type = type;
    p->pid = pid;

    p->parent = parent;

    spinlock_init(&p->plock);
    spinlock_init(&p->files_lock);

    /* Init thread list */
    klist_head_init(&p->threads);
    klist_head_init(&p->children);
}

static pid_t process_new_pid(void)
{
    struct process *process;
    int used;
    pid_t pid = 0;

    for (; pid < PROCESS_MAX_PID; ++pid)
    {
        used = 0;

        klist_for_each_elem(&processes, process, list)
        {
            if (process->pid == pid)
            {
                used = 1;
                break;
            }
        }

        if (!used)
            return pid;
    }

    return -1;
}

struct process *process_create(int type, uintptr_t code, int flags)
{
    struct process *process;
    pid_t pid;

    if ((pid = process_new_pid()) < 0)
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

    /*
     * Process created by this function are processes that are launched as
     * module at boot time, the parent is logically init
     */
    init_process(process, pid, type, process_get(1));

    memset(process->files, 0, sizeof (process->files));

    if (flags & PROCESS_FLAG_LOAD)
    {
        code = process_load_elf(process, code);

        if (!code)
            goto error;
    }

    if (thread_create(process, code, NULL, 0) < 0)
        goto error;

    klist_add(&processes, &process->list);

    return process;

error:
    if (process->as != &kernel_as)
        as_destroy(process->as);

    kfree(process);

    return NULL;
}

struct process *process_get(pid_t pid)
{
    struct process *process;

    klist_for_each_elem(&processes, process, list)
    {
        if (process->pid == pid)
            return process;
    }

    return NULL;
}

int process_fork(struct process *process, struct irq_regs *regs)
{
    struct process *child;
    pid_t pid = process_new_pid();

    if (!pid)
        return -EAGAIN;

    child = kmalloc(sizeof (struct process));

    if (!child)
        return -ENOMEM;

    if (!(child->as = as_duplicate(process->as)))
    {
        kfree(child);

        return -EAGAIN;
    }

    init_process(child, pid, process->type, process);

    if (!thread_duplicate(child, thread_current(), regs))
    {
        as_destroy(child->as);

        kfree(child);

        return -ENOMEM;
    }

    memcpy(child->files, process->files, sizeof (process->files));

    klist_add(&process->children, &child->brothers);

    klist_add(&processes, &child->list);

    return pid;
}

int process_new_fd(struct process *process)
{
    spinlock_lock(&process->files_lock);

    for (int i = 0; i < PROCESS_MAX_OPEN_FD; ++i)
    {
        if (!process->files[i].used)
        {
            process->files[i].used = 1;

            spinlock_unlock(&process->files_lock);

            return i;
        }
    }

    spinlock_unlock(&process->files_lock);

    return -EMFILE;
}

int process_fd_exist(struct process *process, int fd)
{
    int ret;

    if (fd < 0 || fd >= PROCESS_MAX_OPEN_FD)
        return 0;

    spinlock_lock(&process->files_lock);

    ret = process->files[fd].used;

    spinlock_unlock(&process->files_lock);

    return ret;
}

void process_free_fd(struct process *process, int fd)
{
    if (fd < 0 || fd >= PROCESS_MAX_OPEN_FD)
        return;

    spinlock_lock(&process->files_lock);

    process->files[fd].used = 0;

    spinlock_unlock(&process->files_lock);
}

void process_exit(struct process *p, int code)
{
    struct cpu *cpu = cpu_get(cpu_id_get());
    struct thread *thread;

    spinlock_lock(&p->plock);

    p->exit_state = code;
    p->state = PROCESS_STATE_ZOMBIE;

    spinlock_unlock(&p->plock);

    /*
     * Exit all threads. When a process have no thread anymore it will be
     * destroyed
     */

    klist_for_each(&p->threads, tlist, list)
    {
        thread = klist_elem(tlist, struct thread, list);

        thread_exit(thread);
    }

    cpu->scheduler.time = 1;

    scheduler_update(NULL, 1);
}

void process_destroy(struct process *p)
{
    (void) p;

    if (p->pid == 1)
        kernel_panic("Init has been killed");

    as_destroy(p->as);

    kfree(p->as);

    spinlock_lock(&p->plock);

    scheduler_event_notify(SCHED_EV_PEXIT, p->pid);

    scheduler_event_notify(SCHED_EV_PEXIT_PARENT, p->parent->pid);

    spinlock_unlock(&p->plock);
}
