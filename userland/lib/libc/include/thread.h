#ifndef LIBC_THREAD_H
# define LIBC_THREAD_H

void thread_exit(void);
int thread_create(void (*callback)(void *), void *arg);

#endif /* !LIBC_THREAD_H */
