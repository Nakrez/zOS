#ifndef ARCH_I386_THREAD_H
# define ARCH_I386_THREAD_H

# include <kernel/thread.h>

int i386_thread_create(struct process *p, struct thread *t, uintptr_t eip);

#endif /* !ARCH_I386_THREAD_H */
