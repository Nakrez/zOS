#include <kernel/errno.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

static void remove_process(struct process *child)
{
    klist_del(&child->brothers);
    klist_del(&child->list);

    child->pid = -1;
}

static pid_t find_child(struct process *p, pid_t pid, int *status)
{
    struct process *child;

    if (!(child = process_get(pid)))
        return -ECHILD;

    if (child->parent != p)
        return -ECHILD;

    spinlock_lock(&child->plock);

    if (child->state == PROCESS_STATE_ZOMBIE)
        spinlock_unlock(&child->plock);
    else
        thread_block(thread_current(), SCHED_EV_PEXIT, pid, &child->plock);

    if (status)
        *status = child->exit_state;

    remove_process(child);

    return pid;
}

pid_t process_waitpid(struct process *p, pid_t pid, int *status, int options)
{
    (void) options;

    pid_t ret;

    if ((ret = find_child(p, pid, status)) < 0)
        return ret;

    return ret;
}
