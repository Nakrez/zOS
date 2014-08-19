#include <event.h>

#include <arch/syscall.h>

int event_register(int event)
{
    int ret;

    SYSCALL1(SYS_EVENT_REGISTER, event, ret);

    return ret;
}
