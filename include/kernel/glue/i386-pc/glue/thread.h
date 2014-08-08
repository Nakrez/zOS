#ifndef GLUE_I386_PC_THREAD_H
# define GLUE_I386_PC_THREAD_H

#include <kernel/thread.h>

int i386_pc_thread_create(struct process *p, struct thread *t, uintptr_t eip);

#endif /* !GLUE_I386_PC_THREAD_H */
