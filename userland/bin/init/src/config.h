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
 * \file    userland/bin/init/src/config.h
 * \brief   Function prototypes and structure declaration for init
 *          configuration parser
 *
 * \author  Baptiste Covolato
 */


#ifndef INIT_CONFIG_H
# define INIT_CONFIG_H

/**
 * \def INIT_HIGHEST_PRIO
 * The highest priority an entry can have
 * \def INIT_LOWEST_PRIO
 * The lowest priority an entry can have
 */
# define INIT_HIGHEST_PRIO 0
# define INIT_LOWEST_PRIO 20

/**
 * \brief   The number of priority available
 */
# define INIT_NB_PRIO ((INIT_LOWEST_PRIO + 1) - INIT_HIGHEST_PRIO)

/**
 * \brief
 */
struct init_prio_entry {
    /**
     * \brief   The binary path to launch
     */
    char *bin;

    /**
     * \brief   The argument to pass to the binary
     */
    char *argv;

    /**
     * \brief   The file to open for stdin, stdout, stderr
     */
    char *tty;

    /**
     * \brief   Device created by the binary that can init will wait for it to
     *          be available before executing the next level of priority
     */
    char *device;

    /**
     * \brief   This field is set if the entry has been launched
     */
    int launched;

    /**
     * \brief   This field is set if the entry has been launched and is ready
     */
    int ready;

    /**
     * \brief   Pointer to the next entry
     */
    struct init_prio_entry *next;
};

/**
 * \brief
 */
struct init_conf {
    struct init_prio_entry *entries[INIT_NB_PRIO];

    int number_of_entries;
};

/**
 * \brief   Create a new init configuration
 *
 * \return  The new configuration structure if success
 * \return  NULL: an error occurred
 */
struct init_conf *init_conf_create(void);

/**
 * \brief   Read a configuration file from a file descriptor
 *
 * \param   conf_fd The file descriptor to read the configuration from
 * \param   conf    The configuration output
 *
 * \return  0: No error
 * \return  -1: Error
 */
int init_conf_parse(int conf_fd, struct init_conf *conf);

/**
 * \brief   Destroy a configuration (deallocate memory)
 *
 * \param   conf    The configuration you want to destroy
 */
void init_conf_destroy(struct init_conf *conf);

#endif /* !CONFIG_H */
