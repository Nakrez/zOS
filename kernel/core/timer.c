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
 * \file    kernel/core/timer.c
 * \brief   Function implementation for timer management
 *
 * \author  Baptiste Covolato
 */

#include <kernel/zos.h>
#include <kernel/errno.h>
#include <kernel/panic.h>
#include <kernel/time.h>
#include <kernel/timer.h>
#include <kernel/cpu.h>
#include <kernel/scheduler.h>

#include <kernel/mem/kmalloc.h>

static struct timer_entry timers[TIMER_NUM];
spinlock_t timer_lock;

void timer_initialize(void)
{
    if (glue_call(timer, init) < 0)
        kernel_panic("Failed to initialize timers");

    for (int i = 0; i < TIMER_NUM; ++i)
        timers[i].free = 1;

    spinlock_init(&timer_lock);
}

void timer_handler(struct irq_regs *regs)
{
    struct timer_entry *timer;
    struct cpu *cpu = cpu_get(cpu_id_get());

    ++ticks;

    klist_for_each(&cpu->timers, tlist, list) {
        timer = klist_elem(tlist, struct timer_entry, list);

        if (timer->next == ticks) {
            timer->callback(timer->data);

            if (timer->type & TIMER_PERIODIC)
                timer->next = timer_ticks_get() + timer->value;
            else {
                klist_del(&timer->list);
                timer->free = 1;
            }
        }
    }

    scheduler_update(regs, 0);
}

static int timer_new(void)
{
    for (int i = 0; i < TIMER_NUM; ++i)
        if (timers[i].free)
            return i;

    return -1;
}

int timer_register(int type, tick_t time, timer_callback_t callback, long data)
{
    struct cpu *cpu = cpu_get(cpu_id_get());
    int timer;

    if (type != TIMER_ONESHOT && type != TIMER_PERIODIC)
        return -EINVAL;

    if (!callback)
        return -EINVAL;

    if (!time)
        return -EINVAL;

    spinlock_lock(&timer_lock);

    timer = timer_new();

    if (timer < 0) {
        spinlock_unlock(&timer_lock);

        return -1;
    }

    timers[timer].free = 0;

    spinlock_unlock(&timer_lock);

    timers[timer].type = type;
    timers[timer].value = time;
    timers[timer].next = timer_ticks_get() + time;
    timers[timer].callback = callback;
    timers[timer].data = data;

    klist_add(&cpu->timers, &timers[timer].list);

    return 1;
}
