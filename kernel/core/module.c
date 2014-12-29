#include <string.h>

#include <kernel/errno.h>
#include <kernel/types.h>
#include <kernel/console.h>
#include <kernel/panic.h>
#include <kernel/module.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/process.h>

static void skip_space(const char **s)
{
    while (**s == ' ' || **s == '\t')
        ++(*s);
}

static int add_argv(char ***argv, const char *arg, size_t arg_size,
                    size_t *argv_size, size_t *argv_max_size)
{
    if (*argv_size == *argv_max_size)
    {
        char **tmp = krealloc(*argv, sizeof (char *) * *argv_max_size * 2);

        if (!tmp)
            return -ENOMEM;

        *argv_max_size *= 2;
        *argv = tmp;

        memset(*argv + *argv_size, 0, *argv_max_size - *argv_size);
    }

    if (arg)
    {
        if (!((*argv)[*argv_size] = kmalloc(arg_size + 1)))
            return -ENOMEM;

        memcpy((*argv)[*argv_size], arg, arg_size);

        (*argv)[*argv_size][arg_size] = 0;
    }
    else
        (*argv)[*argv_size] = NULL;

    ++(*argv_size);

    return 0;
}

static int split_args(const char *cmdline, char ***argv)
{
    size_t argv_size = 0;
    size_t argv_max_size = 1;
    const char *arg_begin;

    if (!(*argv = kmalloc(sizeof (char *) * argv_max_size)))
        return -ENOMEM;

    memset(*argv, 0, sizeof (char *) * argv_max_size);

    /* 6 = boot-module */
    if (add_argv(argv, "boot-module", 11, &argv_size, &argv_max_size) < 0)
        goto error;

    while (*cmdline)
    {
        skip_space(&cmdline);

        arg_begin = cmdline;

        while (*cmdline && *cmdline != ' ' && *cmdline != '\t')
            ++cmdline;

        if (add_argv(argv, arg_begin, cmdline - arg_begin, &argv_size,
                     &argv_max_size) < 0)
            goto error;
    }

    if (add_argv(argv, NULL, 0, &argv_size, &argv_max_size) < 0)
        goto error;

    return 0;

error:
    if (argv)
    {
        for (int i = 0; (*argv)[i]; ++i)
            kfree((*argv)[i]);

        kfree(*argv);
    }

    return -ENOMEM;
}

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

            if (split_args(boot->mods[i].mod_args, &argv) < 0)
                kernel_panic("Not enough memory to setup command line");
        }
        else
            argv = NULL;

        p = process_create(PROCESS_TYPE_USER,
                           (uintptr_t)boot->mods[i].mod_start,
                           PROCESS_FLAG_LOAD, argv);

        if (!p)
            kernel_panic("Fail to load module");

        /* FIXME: Deallocate argv properly */
        kfree(argv);
    }
}
