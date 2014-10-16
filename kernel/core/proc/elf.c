#include <string.h>

#include <kernel/mem/region.h>
#include <kernel/mem/segment.h>
#include <kernel/mem/as.h>

#include <kernel/proc/elf.h>
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

#include <arch/mmu.h>

int is_elf(void *elf)
{
    Elf32_Ehdr *hdr = elf;

    return (hdr->e_ident[EI_MAG0] == ELFMAG0 &&
            hdr->e_ident[EI_MAG1] == ELFMAG1 &&
            hdr->e_ident[EI_MAG2] == ELFMAG2 &&
            hdr->e_ident[EI_MAG3] == ELFMAG3);
}

uintptr_t process_load_elf(struct process *p, uintptr_t elf)
{
    int need_kernel = 0;
    Elf32_Ehdr *hdr = (void *)elf;
    Elf32_Phdr *phdr = (void *)((char *)hdr + hdr->e_phoff);
    struct thread *t = thread_current();

    /*
     * If the process address space is not the current one we need extra
     * mapping in the kernel to write on the pages
     */
    if (!t || t->parent->as != p->as)
        need_kernel = 1;

    for (uint32_t i = 0; i < hdr->e_phnum; ++i)
    {
        if (phdr[i].p_type != PT_LOAD)
            continue;

        vaddr_t vaddr;
        paddr_t paddr;
        size_t page_size = align(phdr[i].p_memsz, PAGE_SIZE) / PAGE_SIZE;

        /* TODO: Error handling */
        vaddr = region_reserve(p->as, phdr[i].p_vaddr, page_size);

        /* TODO: Error handling */
        paddr = segment_alloc_address(page_size);

        /* TODO: Error handling */
        vaddr = as_map(p->as, vaddr, paddr, phdr[i].p_memsz,
                       AS_MAP_USER | AS_MAP_WRITE);

        /* TODO: Error handling */
        if (need_kernel)
            vaddr = as_map(&kernel_as, 0, paddr, phdr[i].p_memsz,
                           AS_MAP_WRITE);

        memcpy((void *)vaddr, (void *)(elf + phdr[i].p_offset),
               phdr[i].p_filesz);

        memset((char *)vaddr + phdr[i].p_filesz, 0,
               phdr[i].p_memsz - phdr[i].p_filesz);

        if (need_kernel)
            as_unmap(&kernel_as, vaddr, AS_UNMAP_NORELEASE);
    }

    return hdr->e_entry;
}
