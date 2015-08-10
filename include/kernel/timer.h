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
 * \file    include/kernel/timer.h
 * \brief   Function prototypes for timer management
 *
 * \author  Baptiste Covolato
 */

#ifndef TIMER_H
# define TIMER_H

# include <kernel/types.h>
# include <kernel/klist.h>

# include <arch/spinlock.h>

# include <arch/cpu.h>

# define TIMER_NUM 255

# define TIMER_ONESHOT 0
# define TIMER_PERIODIC 1

typedef void (*timer_callback_t)(int);

struct timer_glue {
    int (*init)(void);
};

/**
 *  \brief  Represents a timer
 */
struct timer_entry {
    /**
     *  \brief  Tell if the timer is free
     */
    int free;

    /**
     *  \brief  The type of the counter
     */
    int type;

    /**
     *  \brief  The initial value of the timer (useful for periodic timers)
     */
    tick_t value;

    /**
     *  \brief  The next time (in ticks) the timer will overflow
     */
    tick_t next;

    /**
     *  \brief  The callback to call when the timer expires
     */
    timer_callback_t callback;

    /**
     *  \brief  This data will be passed to the callback function
     */
    long data;

    struct klist list;
};

extern struct timer_glue timer_glue_dispatcher;

/**
 *  \brief  Initialize the timer mechanism
 */
void timer_initialize(void);

/**
 *  \brief  This handler is called by the timer interrupt of the system
 *
 *  \param  regs    Architecture dependent registers (it is called in an
 *                  interrupt context)
 */
void timer_handler(struct irq_regs *regs);

/**
 *  \brief  Register a new timer
 *
 *  \param  type        Specify if the timer is periodic or a one shot
 *  \param  time        The time (in tick) the timer must expire
 *  \param  callback    The callback to call when the timer expires
 *  \param  data        The data to pass to the callback
 *
 *  \return 0: Everything went well
 *  \return -EINVAL: Invalid argument
 */
int timer_register(int type, tick_t time, timer_callback_t callback,
                   long data);

#endif /* !TIMER_H */
