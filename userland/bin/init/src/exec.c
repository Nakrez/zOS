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

static int exec_entry(struct init_prio_entry *entry)
{
    int in = -1;
    int out = -1;
    int err = -1;
    int pid;
    char buf[100];

    sprintf(buf, "Init: Launching %s", entry->bin);
    uprint(buf);

    if (strlen(entry->tty) > 0) {
        in = open_device(entry->tty, O_RDWR, 0);
        if (in < 0)
            goto error;

        out = dup2(in, STDOUT_FILENO);
        if (out < 0)
            goto error;

        err = dup2(in, STDERR_FILENO);
        if (err < 0)
            goto error;
    }

    pid = fork();
    if (pid < 0)
        goto error;

    if (pid == 0) {
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

static int conf_exec_level(struct init_conf *config, int level)
{
    int count;
    int count_ready;
    char buf[100];

    sprintf(buf, "Executing run level %d", level);
    uprint(buf);

    do {
        struct init_prio_entry *entry;

        entry = config->entries[level];

        count = 0;
        count_ready = 0;

        while (entry) {
            if (!entry->launched) {
                /* If we are ready to launch the binary do it */
                if (strlen(entry->tty) == 0 ||
                    device_exists(entry->tty)) {
                    if (exec_entry(entry) < 0)
                        return -1;

                    entry->launched = 1;
                }
            }

            if (!entry->ready) {
                /* Check if this entry is up and running */
                if (!entry->device ||
                    device_exists(entry->device)) {
                    entry->ready = 1;

                    ++count_ready;
                }
            } else {
                ++count_ready;
            }

            entry = entry->next;
            ++count;
        }

        /* No need to busy wait. Let time to the binary to be launched.
         * XXX: Ideally use some event mechanism
         */
        usleep(100);
    }
    while (count != count_ready);

    return 0;
}

int init_conf_execute(struct init_conf *config)
{
    for (int i = INIT_HIGHEST_PRIO; i <= INIT_LOWEST_PRIO; ++i) {
        int ret;

        if (!config->entries[i])
            continue;

        ret = conf_exec_level(config, i);
        if (ret < 0)
            return ret;
    }

    return 0;
}
