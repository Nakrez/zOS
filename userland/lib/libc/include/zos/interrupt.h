#ifndef LIBC_ZOS_INTERRUPT_H
# define LIBC_ZOS_INTERRUPT_H

int interrupt_register(int interrupt);
int interrupt_listen(int interrupt);
void interrupt_unregister(int interrupt);

#endif /* !LIBC_ZOS_INTERRUPT_H */
