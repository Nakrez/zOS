#ifndef GLUE_I386_PC_AS_H
# define GLUE_I386_PC_AS_H

# include <kernel/types.h>

struct as;

struct glue_as
{
    paddr_t cr3;
};

int i386_pc_as_initialize(struct as *as);
int i386_pc_as_map(vaddr_t vaddr, paddr_t paddr, size_t size, int flags);
void i386_pc_as_unmap(vaddr_t vaddr, size_t size);

#endif /* !GLUE_I386_PC_AS_H */
