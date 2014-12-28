/*
 * zOS
 * Copyright (C) 2014 - 2015 Baptiste Covolato
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zOS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file    kernel/core/proc/process.c
 * \brief   Implementation of some process management related functions
 *
 * \author  Baptiste Covolato
 */

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
    pid_t pid;
    struct process *process;

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

    /* Mark all file slots has unused */
    memset(process->files, 0, sizeof (process->files));

    /* Load the binary pointed by code if necessary */
    if (flags & PROCESS_FLAG_LOAD)
    {
        code = process_load_elf(process, code);

        if (!code)
            goto error;
    }

    /* Create main thread */
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
    pid_t pid;
    struct process *child;

    if (!(pid = process_new_pid()))
        return -EAGAIN;

    if (!(child = kmalloc(sizeof (struct process))))
        return -ENOMEM;

    if (!(child->as = as_duplicate(process->as)))
    {
        kfree(child);

        return -EAGAIN;
    }

    init_process(child, pid, process->type, process);

    /*
     * Duplicate thread, the child's thread will automatically be added to the
     * scheduler and return in the userland code and return 0 to the syscall
     * fork() performed by the father
     */
    if (!thread_duplicate(child, thread_current(), regs))
    {
        as_destroy(child->as);

        kfree(child);

        return -ENOMEM;
    }

    /* Inherit file descriptor from parent */
    memcpy(child->files, process->files, sizeof (process->files));

    /* Add the new process to the father's children list */
    klist_add(&process->children, &child->brothers);

    /* Add the process to the process' list */
    klist_add(&processes, &child->list);

    /* Only the father returns here, the child return in thread_duplicate */
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

    /* Notify process waiting for this process to exit, if there is any */
    scheduler_event_notify(SCHED_EV_PEXIT, p->pid);

    scheduler_event_notify(SCHED_EV_PEXIT_PARENT, p->parent->pid);

    spinlock_unlock(&p->plock);
}
