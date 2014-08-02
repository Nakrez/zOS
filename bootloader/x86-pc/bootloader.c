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

#include <boot/boot.h>
#include <boot/multiboot.h>

#include <kernel/elf.h>

#include "utils.h"

# define K_PADDR(addr) ((void*)(addr - 0xC0000000))
# define K_VADDR(addr) ((void*)(addr + 0xC0000000))

static void *boot_brk = 0;

static void *boot_alloc(size_t size)
{
    void *addr = boot_brk;

    boot_brk += size;

    return addr;
}

static void setup_modules(struct boot_info *b_inf, multiboot_info_t *multiboot)
{
    multiboot_module_t *mods = (multiboot_module_t*)multiboot->mods_addr;

    b_inf->mods_count = multiboot->mods_count;

    b_inf->mods = boot_alloc(b_inf->mods_count * sizeof (struct boot_modules));

    for (size_t i = 1; i < multiboot->mods_count; ++i)
    {
        b_inf->mods[i].mod_size = mods[i].mod_end - mods[i].mod_start;
        b_inf->mods[i].mod_start = boot_alloc(b_inf->mods[i].mod_size);

        memcpy(b_inf->mods[i].mod_start, (void *)mods[i].mod_start,
               b_inf->mods[i].mod_size);

        b_inf->mods[i].mod_start = K_VADDR(b_inf->mods[i].mod_start);
    }
}

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

    void *kentry;
    multiboot_module_t *mods = (multiboot_module_t*)multiboot->mods_addr;
    struct boot_info *b_inf;

    /* Load kernel */
    kentry = load_kernel((Elf32_Ehdr *)mods[0].mod_start);

    b_inf = boot_alloc(sizeof (struct boot_info));

    /* Setup boot info with module infos */
    setup_modules(b_inf, multiboot);

    while (1)
        ;
}
