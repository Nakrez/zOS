#include <unistd.h>
#include <string.h>
#include <thread.h>

#include <zos/print.h>

#include <arch/interrupt.h>

int main(void)
{
    int arch_tid;

    arch_tid = thread_create(interrupt_thread, NULL);

    if (arch_tid < 0)
    {
        uprint("Cannot create keyboard interrupt thread ...");
        return 1;
    }

    while (1)
        sleep(1);

    return 0;
}
