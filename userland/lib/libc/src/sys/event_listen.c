#include <event.h>

#include <arch/syscall.h>

int event_listen(void)
{
    int ret;

    SYSCALL0(SYS_EVENT_LISTEN, ret);

    return ret;
}
