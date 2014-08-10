#include <kernel/scheduler.h>

#include <arch/scheduler.h>

struct scheduler_glue _scheduler =
{
    i386_switch,
};
