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
 * \file    userland/bin/init/src/config.c
 * \brief   Implementation of init configuration parser
 *
 * \author  Baptiste Covolato
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include <zos/print.h>

#include "config.h"

struct init_conf *init_conf_create(void)
{
    struct init_conf *conf;

    if (!(conf = malloc(sizeof (struct init_conf))))
        return NULL;

    conf->number_of_entries = 0;

    memset(conf->entries, 0, sizeof (conf->entries));

    return conf;
}

static int read_conf_from_fd(int conf_fd, char **config_text)
{
    int ret;
    char *tmp;
    struct stat stat;

    if (fstat(conf_fd, &stat) < 0)
        return -1;

    if (!(*config_text = malloc(stat.st_size + 1)))
        return -1;

    tmp = *config_text;

    while (stat.st_size > 0)
    {
        if ((ret = read(conf_fd, tmp, stat.st_size)) < 0)
        {
            free(*config_text);

            return -1;
        }

        tmp += ret;
        stat.st_size -= ret;
    }

    *tmp = 0;

    return 0;
}

static int import_entry(char *entry, struct init_conf *conf)
{
    /*
     * Entry format: priority,binary,argv,tty,device_created
     * priority: Between INIT_HIGHEST_PRIO - INIT_LOWEST_PRIO
     * binary: path
     * tty: file to open and to assign to stdin, stdout, stderr
     * device_created: init will wait for this device to be created before
     * executing the next priority level
     */
    int priority;
    char *save_end;
    char *priority_str;
    char *binary_str;
    char *argv_str;
    char *tty_str;
    char *device_str;
    struct init_prio_entry *init_entry;

    priority_str = strtok_r(entry, ",", &save_end);
    binary_str = strtok_r(NULL, ",", &save_end);
    argv_str = strtok_r(NULL, ",", &save_end);
    tty_str = strtok_r(NULL, ",", &save_end);
    device_str = save_end;

    if (!priority_str || !binary_str || !argv_str || !tty_str)
    {
        uprint("Init: Invalid entry format");

        return -1;
    }

    priority = strtol(priority_str, &save_end, 10);

    if (*save_end != '\0' || priority > INIT_LOWEST_PRIO ||
        priority < INIT_HIGHEST_PRIO)
    {
        uprint("Init: Invalid priority");

        return -1;
    }

    if (!(init_entry = malloc(sizeof (struct init_prio_entry))))
        goto mem_error;

    init_entry->bin = NULL;
    init_entry->argv = NULL;
    init_entry->tty = NULL;
    init_entry->device = NULL;
    init_entry->launched = 0;
    init_entry->ready = 0;

    if (!(init_entry->bin = strdup(binary_str)))
        goto mem_error;

    if (!(init_entry->argv = strdup(argv_str)))
        goto mem_error;

    if (!(init_entry->tty = strdup(tty_str)))
        goto mem_error;

    if (device_str && strlen(device_str) > 0)
    {
        if (!(init_entry->device = strdup(device_str)))
            goto mem_error;
    }

    init_entry->next = conf->entries[priority];
    conf->entries[priority] = init_entry;

    return 0;

mem_error:
    uprint("Init: Not enough memory");

    if (init_entry)
    {
        free(init_entry->bin);
        free(init_entry->argv);
        free(init_entry->tty);
        free(init_entry->device);
        free(init_entry);
    }

    return -1;
}

int init_conf_parse(int conf_fd, struct init_conf *conf)
{
    int end = 0;
    char *cur;
    char *entry;
    char *config_text;

    if (read_conf_from_fd(conf_fd, &config_text) < 0)
        return -1;

    cur = config_text;

    while (!end)
    {
        entry = cur;

        while (*cur && *cur != '\n')
            ++cur;

        if (!*cur)
            end = 1;

        *cur = 0;

        /* Skip leading whitespace */
        while (*entry == '\t' || *entry == ' ')
            ++entry;

        /* Empty line */
        if (cur == entry)
        {
            ++cur;
            continue;
        }

        ++cur;

        /* Comment */
        if (*entry == '#')
            continue;

        if (import_entry(entry, conf) < 0)
        {
            free(config_text);

            return -1;
        }

    }

    free(config_text);

    return 0;
}

void init_conf_destroy(struct init_conf *conf)
{
    struct init_prio_entry *entry;
    struct init_prio_entry *tmp;

    if (!conf)
        return;

    for (int i = INIT_HIGHEST_PRIO; i <= INIT_LOWEST_PRIO; ++i)
    {
        if (conf->entries[i])
        {
            entry = conf->entries[i];

            while (entry)
            {
                tmp = entry->next;

                free(entry->bin);
                free(entry->argv);
                free(entry->tty);
                free(entry->device);
                free(entry);

                entry = tmp;
            }
        }
    }

    free(conf);
}
