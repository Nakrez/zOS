#ifndef PARTITION_H
# define PARTITION_H

# include <stdint.h>

# define PARTITION_OFFSET 0x1BE

struct partition {
    int present;

    size_t start;

    size_t size;
};

void partition_initialize(struct partition *parts, void *mbr);

#endif /* !PARTITION_H */
