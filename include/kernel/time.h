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
 * \file    include/kernel/time.h
 * \brief   Function prototypes for time management
 *
 * \author  Baptiste Covolato
 */

#ifndef TIME_H
# define TIME_H

# include <kernel/types.h>

# define MSEC_PER_SEC 1000

# define TICK_PER_SEC 1000

/**
 *  \brief  This counter is incremented every time a timer interrupt occurs
 */
extern tick_t ticks;

/**
 *  \brief  Return the value of ticks. This function is here to abstract the
 *          ticks variable to prevent API changes
 */
static inline tick_t timer_ticks_get(void)
{
    return ticks;
}

/**
 *  \brief  Convert milliseconds to ticks
 *
 *  \return The number of ticks converted
 */
tick_t ms_to_ticks(unsigned int ms);

/**
 *  \brief  Convert a number of tick into millisecond
 *
 *  \return The number of milliseconds converted
 */
unsigned int ticks_to_ms(tick_t tick);

#endif /* !TIME_H */
