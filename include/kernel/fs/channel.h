/*
 * zOS
 * Copyright (C) 2015 Baptiste Covolato
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
 * \file    include/kernel/fs/channel.h
 * \brief   Definition of function and structure related to channel IPC
 *          mechanism
 *
 * \author  Baptiste Covolato
 */

#ifndef FS_CHANNEL_H
# define FS_CHANNEL_H

# include <kernel/types.h>
# include <kernel/klist.h>

# include <kernel/proc/wait_queue.h>

# include <arch/spinlock.h>

# define CHANNEL_NAME_MAXL 20

struct file;

/**
 *  \brief  Message exchanged through a channel
 */
struct channel_message {
    /**
     *  \brief  The id of the slave that need to receive/send a message
     */
    uint16_t cid;

    /**
     *  \brief  The current size of the message
     */
    size_t size;

    /**
     *  \brief  The offset in the data
     */
    size_t off;

    /**
     *  \brief  List of message
     */
    struct klist list;
};

struct channel {
    /**
     *  \brief  The name of the channel
     */
    char name[CHANNEL_NAME_MAXL];

    /**
     *  \brief  Current slave id
     */
    uint16_t slave_id;

    /**
     *  The process that owns the channel
     */
    struct process *proc;

    /**
     *  \brief  Used by a master to wait on the channel
     */
    struct wait_queue wait;

    /**
     *  \brief  Used to protect access of a master
     */
    spinlock_t lock;

    /**
     *  \brief  List of message to be processed
     */
    struct klist input;

    /**
     *  \brief  List of slaves
     */
    struct klist slaves;

    /**
     *  \brief  List of channel
     */
    struct klist list;

};

struct channel_slave {
    /**
     *  \brief  The id of the slave
     */
    uint16_t id;

    /**
     *  \brief  The channel it belongs to
     */
    struct channel *parent;

    /**
     *  \brief  Used to protect access of a slave
     */
    spinlock_t lock;

    /**
     *  The process that owns the slave
     */
    struct process *proc;

    /**
     *  \brief  Used by a slave to wait on the channel
     */
    struct wait_queue wait;

    /**
     *  \brief  List of message to be processed
     */
    struct klist input;

    /**
     *  \brief  List of slaves related to a channel
     */
    struct klist list;
};
/**
 *  \brief  Initialize data structure related to channels
 *
 *  \return 0: Everything went well
 */
int channel_initialize(void);

/**
 *  \brief  Create a new channel
 *
 *  \param  name    The name of the channel
 *  \param  file    The file that represents the channel
 *  \param  channel The resulting channel if success
 *
 *  \return 0:  Everything went well
 *  \return -ENOMEM: Not enough memory
 *  \return -EEXIST: A channel with this name already exists
 */
int channel_create(const char *name, struct file *file,
                   struct channel **channel);

/**
 *  \brief  Open an existing channel
 *
 *  \param  name    The name of the channel to open
 *  \param  file    The file representing the channel's slave
 *  \param  slave   The channel slave
 *
 *  \return 0: Everything went well
 *  \return -ENOENT: No such channel
 *  \return -ENOMEM: Not enough memory
 */
int channel_open(const char *name, struct file *file,
                 struct channel_slave **slave);

#endif /* !FS_CHANNEL_H */
