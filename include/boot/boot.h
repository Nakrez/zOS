/*
Copyright (C) 2013, 2014 Baptiste Covolato <b.covolato@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef BOOT_H
# define BOOT_H

# include <kernel/types.h>

struct boot_seg
{
    uint64_t seg_start;
    uint64_t seg_size;
};

struct boot_modules
{
    void *mod_start;
    uint32_t mod_size;
};

struct boot_info
{
    uintptr_t seg_low;
    uintptr_t seg_high;

    uint32_t segs_count;
    struct boot_segs *segs;

    uint32_t mods_count;
    struct boot_modules *mods;

    void *heap_start;
    uint64_t heap_size;
};

#endif /* !BOOT_H */
