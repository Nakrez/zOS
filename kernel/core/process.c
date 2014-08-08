#include <kernel/process.h>

static struct klist processes;

void process_initialize(void)
{
    klist_head_init(&processes);
}
