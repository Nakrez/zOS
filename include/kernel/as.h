#ifndef AS_H
# define AS_H

# include <kernel/klist.h>

# include <glue/as.h>

struct as
{
    struct klist regions;

    struct glue_as arch;
};

struct as_glue
{
    int (*init)(struct as *as);
};

extern struct as kernel_as;
extern struct as_glue __as;

/*
 * Creates a new address space and returns it
 */
struct as *as_create(void);

/*
 * Initializes a new address space
 */
int as_initialize(struct as* as);

#endif /* !AS_H */
