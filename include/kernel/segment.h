#ifndef SEGMENT_H
# define SEGMENT_H

# include <boot/boot.h>

struct segment_glue
{
    void (*init)(void);
};

extern struct segment_glue __segment;

void segment_initialize(struct boot_info *boot);

#endif /* !SEGMENT_H */
