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
 * \file    include/kernel/mem/kmalloc.h
 * \brief   Function prototypes for kernel internal memory allocator
 *
 * \author  Baptiste Covolato
 */

#ifndef KMALLOC_H
# define KMALLOC_H

#include <boot/boot.h>

/**
 * \brief   Initial kernel stack size
 */
# define KSTACK_SIZE 0x1000

/**
 * \brief   Initialize kernel heap from boot info structure
 *
 * \param   boot    The boot info structure containing kernel initial heap
 *                  location and size
 */
void kmalloc_initialize(struct boot_info *boot);

/**
 * \brief   Allocate memory area of size \a size
 * \todo    Replace kernel panic by heap extension
 *
 * \param   size    The size of the memory area you want to allocate
 *
 * return   The memory area location if everything went well, NULL otherwise
 */
void *kmalloc(size_t size);

/**
 * \brief   Reallocate a memory area (change it size)
 * \todo    Make it cleverer
 *
 * \param   ptr     The pointer to the area you want to realloc. If \a ptr is
 *                  NULL the function behave like kmalloc()
 * \param   new_size    The new size of the area
 *
 * \return  The new area location if everything went well, NULL otherwise
 */
void *krealloc(void *ptr, size_t new_size);

/**
 * \brief   Free an allocated area
 *
 * \param   ptr The area to release. If \a ptr is NULL the function does
 *              nothing
 */
void kfree(void *ptr);

/**
 * \brief   Dump all allocated/free area managed by the kernel allocator
 */
void kmalloc_dump(void);

#endif /* !KMALLOC_H */
