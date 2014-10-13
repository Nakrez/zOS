#include <kernel/proc/process.h>

int process_execve(struct process *process, const char *filename,
                   char *const argv[], char *const envp[])
{
    (void)process;
    (void)filename;
    (void)argv;
    (void)envp;

    return 0;
}
