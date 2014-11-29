#ifndef PROCESS_H
# define PROCESS_H

# include <kernel/types.h>

# include <kernel/mem/as.h>

# include <kernel/vfs/vfile.h>

# include <arch/cpu.h>
# include <arch/spinlock.h>

# define PROCESS_MAX_PID 0xFF
# define PROCESS_MAX_OPEN_FD 255

# define PROCESS_TYPE_KERNEL 0
# define PROCESS_TYPE_USER (1 << 1)

/*
 * If set code is an area of memory that is an ELF binary, otherwise it is
 * interpreted as start address
 */
# define PROCESS_FLAG_LOAD (1 << 1)

# define PROCESS_STATE_ALIVE 0
# define PROCESS_STATE_ZOMBIE 1

# define PROCESS_CODE_SEGV 128

struct process
{
    pid_t pid;

    struct process *parent;

    int state;

    int exit_state;

    int type;

    struct as *as;

    size_t thread_count;

    spinlock_t plock;

    spinlock_t files_lock;

    struct vfile files[PROCESS_MAX_OPEN_FD + 1];

    /* List of threads */
    struct klist threads;

    /* List of children */
    struct klist children;

    /* List of brothers */
    struct klist brothers;

    /* List of process */
    struct klist list;
};

void process_initialize(void);

struct process *process_create(int type, uintptr_t code, int flags);

struct process *process_get(pid_t pid);
int process_fork(struct process *process, struct irq_regs *regs);

int process_new_fd(struct process *process);
int process_fd_exist(struct process *process, int fd);
void process_free_fd(struct process *process, int fd);

int process_execv(struct thread *thread, const char *filename,
                  char *const argv[]);

pid_t process_waitpid(struct process *p, pid_t pid, int *status, int options);

void process_exit(struct process *p, int code);

void process_destroy(struct process *p);

#endif /* !PROCESS_H */
