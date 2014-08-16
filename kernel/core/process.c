#include <string.h>

#include <kernel/zos.h>
#include <kernel/process.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>
#include <kernel/thread.h>
#include <kernel/elf.h>
#include <kernel/region.h>
#include <kernel/segment.h>
#include <kernel/as.h>

#include <arch/mmu.h>

static struct klist processes;

void process_initialize(void)
{
    klist_head_init(&processes);
}

static int process_new_pid(void)
{
    struct process *process;
    int used;
    int pid = 0;

    for (; pid < PROCESS_MAX_PID; ++pid)
    {
        used = 0;

        klist_for_each_elem(&processes, process, list)
        {
            if (process->pid == pid)
            {
                used = 1;
                break;
            }
        }

        if (!used)
            return pid;
    }

    return -1;
}

static uintptr_t process_load_elf(struct process *p, uintptr_t elf)
{
    Elf32_Ehdr *hdr = (void *)elf;
    int need_kernel = 0;

    if (hdr->e_ident[EI_MAG0] != ELFMAG0 || hdr->e_ident[EI_MAG1] != ELFMAG1 ||
        hdr->e_ident[EI_MAG2] != ELFMAG2 || hdr->e_ident[EI_MAG3] != ELFMAG3)
        return 0;

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

struct process *process_create(int type, uintptr_t code, int flags)
{
    struct process *process;
    int pid;

    if ((pid = process_new_pid()) < 0)
        return NULL;

    process = kmalloc(sizeof (struct process));

    if (!process)
        return NULL;

    if (type & PROCESS_TYPE_USER)
    {
        process->as = kmalloc(sizeof (struct as));

        if (!process || !as_initialize(process->as))
            goto error;
    }
    else
        process->as = &kernel_as;

    process->state = PROCESS_STATE_ALIVE;
    process->thread_count = 0;
    process->type = type;
    process->pid = pid;

    /* Init thread list */
    klist_head_init(&process->threads);

    if (flags & PROCESS_FLAG_LOAD)
    {
        code = process_load_elf(process, code);

        if (!code)
            goto error;
    }

    if (!thread_create(process, code))
        goto error;

    klist_add(&processes, &process->list);

    return process;

error:
    /* FIXME: Add as_delete */
    if (process->as != &kernel_as)
        kfree(process->as);

    return NULL;
}

void process_exit(struct process *p, int code)
{
    struct thread *thread;

    p->exit_state = code;
    p->state = PROCESS_STATE_ZOMBIE;

    /*
     * Exit all threads. When a process have no thread anymore it will be
     * destroyed
     */

    klist_for_each(&p->threads, tlist, list)
    {
        thread = klist_elem(tlist, struct thread, list);

        thread_exit(thread);
    }
}

void process_destroy(struct process *p)
{
    (void) p;

    as_destroy(p->as);

    kfree(p->as);
}
