#ifndef LIBC_EVENT_H
# define LIBC_EVENT_H

int event_register(int event);
int event_listen(void);
void event_unregister(int event);

#endif /* !LIBC_EVENT_H */
