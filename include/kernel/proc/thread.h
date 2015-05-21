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
 * \file    include/kernel/proc/thread.h
 * \brief   Function prototypes for thread management
 *
 * \author  Baptiste Covolato
 */

#ifndef THREAD_H
# define THREAD_H

# include <kernel/zos.h>
# include <kernel/types.h>
# include <kernel/klist.h>
# include <kernel/interrupt.h>

# include <kernel/proc/process.h>
# include <kernel/scheduler/event.h>

# include <arch/cpu.h>

# define THREAD_CREATEF_NOSTART_THREAD (1 << 0)
# define THREAD_CREATEF_DEEP_ARGV_COPY (1 << 1)

/**
 * \brief   Maxium number of thread per process
 */
# define THREAD_MAX_PER_PROCESS 10

/**
 * \def THREAD_STATE_RUNNING
 * State that indicate that the thread is running
 *
 * \def THREAD_STATE_BLOCKED
 * State that indicate that the thread is blocked
 *
 * \def THREAD_STATE_ZOMBIE
 * State that indicate that the thread is about to die
 */
# define THREAD_STATE_RUNNING 1
# define THREAD_STATE_BLOCKED 2
# define THREAD_STATE_ZOMBIE 3

/**
 * \brief   Internal representation of a thread
 */
struct thread
{
    /**
     * \brief   The current state of the thread
     */
    int state;

    /**
     * \brief   The process that owns the thread
     */
    struct process *parent;

    /**
     * \brief   The user id of the thread
     */
    uid_t uid;

    /**
     * \brief   The group id of the thread
     */
    gid_t gid;

    /**
     * \brief   The thread id
     */
    int tid;

    /**
     * \brief   The cpu the thread run on
     */
    int cpu;

    /**
     * \brief   Used to store the event the thread is waiting on
     */
    struct scheduler_event event;

    /**
     * \brief   Interrupts the thread is listening to
     */
    uint8_t interrupts[IRQ_USER_SIZE];

    /**
     * \brief   Address of the kernel stack
     */
    uintptr_t kstack;

    /**
     * \brief   Use to store architecture dependent registers state
     */
    struct thread_regs regs;

    /**
     * \brief   List of thread in the parent process
     */
    struct klist list;

    /**
     * \brief   List of thread for the scheduler
     */
    struct klist sched;

    /**
     * \brief   List of blocked thread
     */
    struct klist block;
};

/**
 * \brief   This structure is used to dispatch calls to architecture/platform
 *          dependant code
 */
struct thread_glue
{
    /**
     * \brief   Create a new thread
     */
    int (*create)(struct process *p, struct thread *t, uintptr_t ip, int argc,
                  char *argv[], int copy);

    /**
     * \brief   Duplicate a thread
     */
    int (*duplicate)(struct thread *, struct irq_regs *);

    /**
     * \brief   Get the current running thread
     */
    int (*current)(void);

    /**
     * \brief   Save thread state (registers)
     */
    int (*save_state)(struct thread *, struct irq_regs *);
};

/**
 * \brief   Used to dispatch calls to architecture/platform dependant code
 */
extern struct thread_glue thread_glue_dispatcher;

/**
 * \brief   Create a new thread inside a process
 * \todo    Use regular error code (ERRNO)
 *
 * \param   process The process that will be the thread's parent
 * \param   code    The pointer to the thread entry point
 * \param   argc    The number of argument passed to the thread
 * \param   argv    The argument passed to the thread
 * \param   flags   Change thread_create() behaviour
 *
 * \return  The thread id if everything went well, -1 otherwise
 */
int thread_create(struct process *process, uintptr_t code, int argc,
                  char *argv[], int flags);

/**
 *  \brief  Get a thread from a TID
 *
 *  \param  p   The process that contains the thread
 *  \param  tid The thread ID of your thread
 *
 *  \return The thread if it exists, NULL otherwise
 */
struct thread *thread_get(struct process *p, pid_t tid);

/**
 * \brief   Update thread argument and entry point
 * \todo    Use regular error code (ERRNO)
 *
 * \param   thread  The thread you want to update
 * \param   eip     The new entry point of the thread
 * \param   argv    The new arguments of the thread
 *
 * \return  0 if everything went well, -1 otherwise
 */
int thread_update_exec(struct thread *thread, uintptr_t eip, char *argv[]);

/**
 * \brief   Duplicate an existing thread in an other process
 * \todo    Use regular error code (ERRNO)
 *
 * \param   process The new parent of the thread
 * \param   thread  The thread you want to duplicate
 * \param   regs    Register state before the thread entered kernel
 *
 * \return  1 if everything went well, 0 otherwise
 */
int thread_duplicate(struct process *process, struct thread *thread,
                     struct irq_regs *regs);

/**
 * \brief   Get the running thread
 *
 * \return  The running thread, or NULL if there isn't any
 */
static inline struct thread *thread_current(void)
{
    return (struct thread *)glue_call(thread, current);
}

/**
 * \brief   Save the state of a thread (registers)
 *
 * \param   thread  The thread to save
 * \param   regs    The register state to save
 */
void thread_save_state(struct thread *thread, struct irq_regs *regs);

/**
 * \brief   Make a thread sleep for \a ms microseconds
 *
 * \param   thread  The thread to sleep
 * \param   ms      The time in microseconds to sleep
 * \param   regs    The state of the thread
 */
void thread_sleep(struct thread *thread, size_t ms, struct irq_regs *regs);

/**
 * \brief   Block a thread and remove it from scheduler
 *
 * \param   thread  The thread to block
 * \param   event   The event that must wake the thread
 * \param   data    Data associated to the event that wake the thread
 * \param   l       If not NULL the spinlock will be unlocked
 */
void thread_block(struct thread *thread, int event, int data, spinlock_t *l);

/**
 * \brief   Unblock a thread
 *
 * \param   thread  The thread to unblock
 */
void thread_unblock(struct thread *thread);

/**
 * \brief   Exit a thread (The thread will be destroyed the next time it is
 *          elected by the scheduler)
 *
 * \param   thread  The thread to exit
 */
void thread_exit(struct thread *thread);

/**
 * \brief   Destroy the thread (and all associated ressources)
 *
 * \param   thread  The thread to destroy
 */
void thread_destroy(struct thread *thread);

#endif /* !THREAD_H */
