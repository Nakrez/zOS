/*
 * zOS
 * Copyright (C) 2015 Baptiste Covolato
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
 * \file    include/kernel/proc/wait_queue.h
 * \brief   Definition of functions and structures related to wait queues
 *
 * \author  Baptiste Covolato
 */

#ifndef PROC_WAIT_QUEUE_H
# define PROC_WAIT_QUEUE_H

# include <kernel/klist.h>
# include <kernel/cpu.h>

# include <arch/spinlock.h>

/**
 *  \brief  Represent a wait queue
 */
struct wait_queue {
    /**
     *  \brief  Lock protecting the wait queue
     */
    spinlock_t lock;

    /**
     *  \brief List of threads waiting on the queue
     */
    struct klist threads;
};

/**
 *  \brief  Initialize a wait_queue
 *
 *  \param  queue   The queue you want to initialize
 */
static inline void wait_queue_init(struct wait_queue *queue)
{
    klist_head_init(&queue->threads);
    spinlock_init(&queue->lock);
}

/**
 *  \brief  Make a thread wait on a wait queue
 *
 *  \param  queue   The queue you want the thread to wait on
 *  \param  thread  The thread that waits on the queue
 *  \param  cond    Condition to be sure we are the one waken up
 */
# define wait_queue_wait(queue, thread, cond)                       \
    do {                                                            \
        spinlock_lock(&(queue)->lock);                              \
        if (!(cond)) {                                              \
            struct cpu *cpu = cpu_get((thread)->cpu);               \
            thread->state = THREAD_STATE_BLOCKED;                   \
            klist_add_back(&(queue)->threads, &(thread)->wait);     \
            spinlock_unlock(&(queue)->lock);                        \
            spinlock_lock(&cpu->scheduler.sched_lock);              \
            scheduler_remove_thread(thread, &cpu->scheduler);       \
        } else {                                                    \
            spinlock_unlock(&(queue)->lock);                        \
            break;                                                  \
        }                                                           \
    } while (!(cond))

/**
 *  \brief  Wake up the first thread waiting on a queue
 *
 *  \param  queue   The queue you want to notify
 */
static inline void wait_queue_notify(struct wait_queue *queue)
{
    struct thread *thread;

    spinlock_lock(&(queue)->lock);

    thread = klist_first_elem(&(queue)->threads, struct thread, wait);
    if (thread) {
        thread->state = THREAD_STATE_RUNNING;
        cpu_add_thread(thread);
        klist_del(&thread->wait);
    }

    spinlock_unlock(&(queue)->lock);
}

#endif /* !PROC_WAIT_QUEUE_H */
