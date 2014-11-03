#ifndef LIBC_THREAD_H
# define LIBC_THREAD_H

typedef void (*thread_callback_t)(int argc, void *argv[]);

void thread_exit(void);
int thread_create(thread_callback_t entry, int argc, ...);

#endif /* !LIBC_THREAD_H */
