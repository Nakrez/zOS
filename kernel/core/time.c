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
 * \file    kernel/core/time.c
 * \brief   Function implementation for time management
 *
 * \author  Baptiste Covolato
 */

#include <kernel/time.h>

tick_t ticks = 0;

tick_t ms_to_ticks(unsigned int ms)
{
#if TICK_PER_SEC <= MSEC_PER_SEC
    return (MSEC_PER_SEC * ms) / TICK_PER_SEC;
#else
    return (TICK_PER_SEC * tick) / MSEC_PER_SEC;
#endif
}

unsigned int ticks_to_ms(tick_t tick)
{
#if TICK_PER_SEC <= MSEC_PER_SEC
    return (TICK_PER_SEC * tick) / MSEC_PER_SEC;
#else
    return (MSEC_PER_SEC * tick) / TICK_PER_SEC;
#endif
}
