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
 * \file    include/kernel/mem/segment.h
 * \brief   Declaration of functions and structures related to physical memory
 *          management
 *
 * \author  Baptiste Covolato
 */

#ifndef SEGMENT_H
# define SEGMENT_H

# include <boot/boot.h>

# include <kernel/types.h>
# include <kernel/klist.h>

# define SEGMENT_FLAGS_NONE 0
# define SEGMENT_FLAGS_COW 1

struct segment_glue
{
    int (*init)(void);
};

struct segment
{
    /* Start address */
    paddr_t base;

    /* Size in page (for real size multiply by PAGE_SIZE) */
    uint32_t page_size;

    uint8_t flags;

    uint16_t ref_count;

    struct klist list;
};

extern struct segment_glue segment_glue_dispatcher;

/*
 * Initialize segments (physical memory) layout
 * It call glue init member to initialize low level architecture dependent
 * physical memory initialization
 */

void segment_initialize(struct boot_info *boot);

/**
 *  \brief  Add a segment to the list of segment
 *
 *  \param  base        The base address
 *  \param  page_size   The number of page
 *
 *  \return 0: Everything went well
 *  \return -ENOMEM: Not enough memory
 */
int segment_add(uintptr_t base, uint32_t page_size);

/*
 * Allocate page_size pages of physical memory
 *
 * Return the physical address of the segment or 0 if it fails
 */
struct segment *segment_alloc(uint32_t page_size);

paddr_t segment_alloc_address(uint32_t page_size);

/*
 * Reserve physical memory starting at addr and with size (* PAGE_SIZE)
 * page_size
 *
 * Return 1 if it worked, 0 otherwise
 */
int segment_reserve(paddr_t addr, uint32_t page_size);

struct segment *segment_locate(paddr_t addr);

void segment_release(struct segment *seg);

/*
 * Free the segment starting at addr
 */
void segment_free(paddr_t addr);

/*
 * Dump segment free and used
 */
void segment_dump(void);

#endif /* !SEGMENT_H */
