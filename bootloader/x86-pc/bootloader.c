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

#include <boot/multiboot.h>

#include <kernel/elf.h>

#include "utils.h"

# define K_PADDR(addr) ((void*)(addr - 0xC0000000))

static void *boot_brk = 0;

static void *load_kernel(Elf32_Ehdr *khdr)
{
    Elf32_Phdr *kphdr = (Elf32_Phdr*)khdr->e_phoff;

    for (uint32_t i = 0; i < khdr->e_phnum; ++i)
    {
        if (kphdr[i].p_type != PT_LOAD)
            continue;

        void *load_addr = K_PADDR(kphdr[i].p_vaddr);
        void *data = (void *)khdr + kphdr[i].p_offset;
        uint32_t fsize = kphdr[i].p_filesz;
        uint32_t msize = kphdr[i].p_memsz;

        memcpy(load_addr, data, fsize);

        memset(load_addr + fsize, 0, msize - fsize);

        /* Detect higher kernel address to give boot_alloc() a start address */
        if (boot_brk < load_addr + msize)
            boot_brk = load_addr + msize;
    }

    return (void *)khdr->e_entry;
}

void bootloader_entry(unsigned long magic, multiboot_info_t* multiboot)
{
    (void) magic;

    multiboot_module_t *mods = (multiboot_module_t*)multiboot->mods_addr;

    void *kentry = load_kernel((Elf32_Ehdr *)mods[0].mod_start);

    while (1)
        ;
}
