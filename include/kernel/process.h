#ifndef PROCESS_H
# define PROCESS_H

# include <kernel/types.h>
# include <kernel/as.h>

# define PROCESS_MAX_PID 0xFF

struct process
{
    uint8_t pid;

    struct as *as;

    struct klist threads;
};

void process_initialize(void);

#endif /* !PROCESS_H */
