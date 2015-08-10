#ifndef GLUE_I386_PC_AS_H
# define GLUE_I386_PC_AS_H

# include <kernel/types.h>

struct as;

struct glue_as
{
    paddr_t cr3;
};

int i386_pc_as_initialize(struct as *as);
int i386_pc_as_destroy(struct as *as);

#endif /* !GLUE_I386_PC_AS_H */
