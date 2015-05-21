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
 * \file    include/kernel/proc/kthread.h
 * \brief   Function prototypes for the kernel threads management
 *
 * \author  Baptiste Covolato
 */

#ifndef PROC_KTHREAD_H
# define PROC_KTHREAD_H

struct thread;

/**
 *  @brief  Initialize kernel threads
 */
void kthread_initialize(void);

struct thread *kthread_create();

#endif /* !PROC_KTHREAD_H */
