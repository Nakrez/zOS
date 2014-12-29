#include <string.h>

#include <kernel/types.h>
#include <kernel/console.h>
#include <kernel/panic.h>
#include <kernel/module.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/process.h>

void module_initialize(struct boot_info *boot)
{
    struct process *p;
    char **argv = NULL;

    if (!boot->mods_count)
        kernel_panic("No boot modules detected! I am a micro-kernel I can't "
                     "do all the dirty work");

    for (size_t i = 0; i < boot->mods_count; ++i)
    {
        console_message(T_INF, "Loading module %u located at 0x%x, size %u o",
                        i, boot->mods[i].mod_start, boot->mods[i].mod_size);

        if (boot->mods[i].mod_args && strlen(boot->mods[i].mod_args) > 0)
        {
            console_message(T_INF, "Module %u command line: %s", i,
                            boot->mods[i].mod_args);
        }
        else
            argv = NULL;

        p = process_create(PROCESS_TYPE_USER,
                           (uintptr_t)boot->mods[i].mod_start,
                           PROCESS_FLAG_LOAD, argv);

        if (!p)
            kernel_panic("Fail to load module");

        kfree(argv);
    }
}
