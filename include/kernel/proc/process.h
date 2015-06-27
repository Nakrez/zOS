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
 * \file    include/kernel/proc/process.h
 * \brief   Function prototypes for the process management
 *
 * \author  Baptiste Covolato
 */

#ifndef PROCESS_H
# define PROCESS_H

# include <kernel/types.h>

# include <kernel/mem/as.h>

# include <kernel/fs/vfs/file.h>

# include <arch/cpu.h>
# include <arch/spinlock.h>

/**
 * \def PROCESS_MAX_PID
 * Maximum number of process in the system
 *
 * \def PROCESS_MAX_OPEN_FD
 * Maximum number of open file descriptors per process
 */
# define PROCESS_MAX_PID 0xFF
# define PROCESS_MAX_OPEN_FD 255

/**
 * \def PROCESS_TYPE_KERNEL
 * Kernel process. Means it address space is the kernel address space.
 *
 * \def PROCESS_TYPE_USER
 * User process. It address space is different from the kernel address space.
 */
# define PROCESS_TYPE_KERNEL 0
# define PROCESS_TYPE_USER (1 << 1)

/**
 * \brief   Indicate to \a process_load to interpret code field as ELF binary
 */
# define PROCESS_FLAG_LOAD (1 << 1)

/**
 * \def PROCESS_STATE_ALIVE
 * Process is alive
 * \def PROCESS_STATE_ZOMBIE
 * Process is a zombie (waiting for the parent to use wait())
 */
# define PROCESS_STATE_ALIVE 0
# define PROCESS_STATE_ZOMBIE 1

/**
 * \brief   Status returned by a process if it segv
 */
# define PROCESS_CODE_SEGV 128

/**
 * \brief   Represents a process in the kernel
 */
struct process
{
    /**
     * \brief   The pid of the process
     */
    pid_t pid;

    /**
     * \brief   The parent of the process
     */
    struct process *parent;

    /**
     * \brief   The current state of the process (alive/zombie)
     */
    int state;

    /**
     * \brief   The exit status of the process
     * \todo    Rename that field
     */
    int exit_state;

    /**
     * \brief   The type of the process (kernel/userland)
     */
    int type;

    /**
     * \brief   The address space of the process
     */
    struct as *as;

    /**
     * \brief   The number of thread the process has
     */
    size_t thread_count;

    /*
     * \brief   Lock to modify the process
     * \todo    Use it :)
     */
    spinlock_t plock;

    /**
     * \brief   The lock to access \a files filed
     */
    spinlock_t files_lock;

    /**
     * \brief   The file slots
     */
    struct vfile files[PROCESS_MAX_OPEN_FD + 1];

    /**
     * \brief   The list of threads
     */
    struct klist threads;

    /**
     * \brief   The list of children
     */
    struct klist children;

    /**
     * \brief   Used to link process to it brothers
     */
    struct klist brothers;

    /**
     * \brief   Used to link process in the list of process
     */
    struct klist list;
};

/**
 * \brief   Initialize process management
 */
void process_initialize(void);

/**
 * \brief   Create a new process. Only used for modules loaded with the kernel
 *
 * \param   type    The type of the process (user or kernel)
 * \param   code    Memory location of ELF binary, if PROCESS_FLAGS_LOAD is
 *                  set, otherwise interpreted has entry point
 * \param   flags   Change the behaviour of the function
 * \param   argv        The argument to pass to the new binary
 *
 * \return  The new process if everything went well, NULL otherwise
 */
struct process *process_create(int type, uintptr_t code, int flags,
                               char *argv[]);

/**
 * \brief   Get a process from it PID
 *
 * \param   pid     The PID of the process you want to get
 *
 * \return  The process if it PID exists, NULL otherwise
 */
struct process *process_get(pid_t pid);

/**
 * \brief   Create a child process from the process \a process
 *
 * \param   process The process you want to duplicate
 * \param   regs    The copy of the registers state before entering kernel
 *
 * \return  The processus new pid if everything went well.
 * \return  -EAGAIN: The maximum number of PID has been reached or no memory
 *          was available to duplicate MMU dependent structures
 * \return  -ENOMEM: Out of kernel memory
 */
int process_fork(struct process *process, struct irq_regs *regs);

/**
 * \brief   Allocate a new file descriptor for the process \a process
 *
 * \param   process The process you want to allocate a file descriptor on
 *
 * \return  The new file descriptor
 * \return  -EMFILE: The maximum number of file descriptor for the process has
 *          been reached
 */
int process_new_fd(struct process *process);

/**
 * \brief   Check if a file descriptor is in use
 *
 * \param   process The process owning the file descriptor
 * \param   fd      The file descriptor
 *
 * \return  1: The file descriptor is in use
 * \return  0: The file descriptor is free
 */
int process_fd_exist(struct process *process, int fd);

/**
 * \brief   Release a file descriptor
 *
 * \param   process The process owning the file descriptor
 * \param   fd      The file descriptor
 */
void process_free_fd(struct process *process, int fd);

/**
 * \brief   Replace current execution code by a binary contained in \a filename
 *
 * \brief   thread      The thread requesting the execution of \a filename
 * \brief   filename    The path to the new binary
 * \brief   argv        The argument to pass to the new binary
 *
 * \return  This function won't return if it works
 * \return  -ENOMEM: Not enough kernel memory
 * \return  -EACCESS: The binary is now executable
 * \return  -ELIBBAD: The binary is not an ELF
 * \return  Same error code as vfs_stat, vfs_open or vfs_read can occur
 */
int process_execv(struct thread *thread, const char *filename,
                  char *const argv[]);

/**
 * \brief   Wait for a child process with PID \a pid to exit and get it exit
 *          status
 *
 * \brief   p       The process waiting for one of it sons
 * \brief   pid     The pid you want to wait
 * \brief   status  This function will put the exit status in it
 * \brief   options Unused ATM
 *
 * \return  The pid of the process you waited for
 * \return  -ECHILD: The process with PID \a pid was not a child of \a p
 */
pid_t process_waitpid(struct process *p, pid_t pid, int *status, int options);

/**
 * \brief   Exit a process
 *
 * \param   p       The process you want to exit
 * \param   code    The exit status of the process
 */
void process_exit(struct process *p, int code);

/**
 * \brief   Destroy a process. The structure is kept in memory as long as
 *          the process father's has not used wait()
 *
 * \param   p   The process you want to destroy
 */
void process_destroy(struct process *p);

#endif /* !PROCESS_H */
