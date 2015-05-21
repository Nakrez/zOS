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
 * \file    kernel/core/proc/kthread.c
 * \brief   Implementation of kernel thread related functions
 *
 * \author  Baptiste Covolato
 */

#include <kernel/zos.h>
#include <kernel/panic.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>
#include <kernel/proc/kthread.h>

static struct process *kproc = NULL;

void kthread_initialize(void)
{
    kproc = process_create(PROCESS_TYPE_KERNEL, 0, 0, NULL);

    if (!kproc)
        kernel_panic("Fail to initialize kthreads");
}

struct thread *kthread_create(uintptr_t code, int argc, char *argv[])
{
    int tid;
    struct thread *t;

    tid = thread_create(kproc, code, argc, argv,
                        THREAD_CREATEF_NOSTART_THREAD);
    if (tid < 0)
        return NULL;

    t = thread_get(kproc, tid);

    /* XXX: I am not sure this is possible, will see while testing. If this
     * happens it will be worth recovering
     */
    if (!t)
        kernel_panic("Thread get == NULL after thread_create()");

    return t;
}
