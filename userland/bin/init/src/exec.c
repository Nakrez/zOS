/*
 * zOS
 * Copyright (C) 2014 - 2015 Baptiste Covolato
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zOS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file    userland/bin/init/src/exec.c
 * \brief   Function implementation of init configuration execution
 *
 * \author  Baptiste Covolato
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <sys/stat.h>

#include <zos/print.h>
#include <zos/device.h>

#include "exec.h"

static int tty_ready(const char *tty)
{
    struct stat s;

    if (stat(tty, &s) < 0)
        return 0;

    return 1;
}

static int exec_entry(struct init_prio_entry *entry)
{
    int in = -1;
    int out = -1;
    int err = -1;
    int pid;
    char buf[100];

    sprintf(buf, "Init: Launching %s", entry->bin);
    uprint(buf);

    if (strlen(entry->tty) > 0)
    {
        if ((in = open(entry->tty, O_RDWR, 0)) < 0)
            goto error;

        if ((out = dup2(in, STDOUT_FILENO)) < 0)
            goto error;

        if ((err = dup2(in, STDERR_FILENO)) < 0)
            goto error;
    }

    if ((pid = fork()) < 0)
        goto error;

    if (!pid)
    {
        char *argv[] = { entry->bin, NULL };

        if (execv(entry->bin, argv) < 0)
            uprint("Init: execve() failed");

        exit(1);
    }

    close(in);
    close(out);
    close(err);

    return 0;

error:
    close(in);
    close(out);
    close(err);

    return -1;
}

int init_conf_execute(struct init_conf *config)
{
    struct init_prio_entry *entry;

    for (int i = INIT_HIGHEST_PRIO; i <= INIT_LOWEST_PRIO; ++i)
    {
        if (config->entries[i])
        {
            int count;
            int count_ready;

            do
            {
                count = 0;
                count_ready = 0;

                entry = config->entries[i];

                while (entry)
                {
                    if (!entry->launched)
                    {
                        /* If we are ready to launch the binary do it */
                        if (strlen(entry->tty) == 0 ||
                            tty_ready(entry->tty))
                        {
                            if (exec_entry(entry) < 0)
                                return -1;

                            entry->launched = 1;
                        }
                    }
                    else if (!entry->ready)
                    {
                        /* Check if this entry is up and running */
                        if (!entry->device ||
                            device_exists(entry->device))
                        {
                            entry->ready = 1;

                            ++count_ready;
                        }
                    }
                    else
                        ++count_ready;

                    entry = entry->next;
                    ++count;
                }
            }
            while (count != count_ready);
        }
    }

    return 0;
}
