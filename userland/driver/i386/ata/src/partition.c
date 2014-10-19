#include "partition.h"

struct partition_entry {
    uint8_t bootable;

    uint8_t start_head;
    uint16_t start_sector : 6;
    uint16_t start_cylinder : 10;

    uint8_t type;

    uint8_t end_head;
    uint16_t end_sector : 6;
    uint16_t end_cylinder : 10;

    uint32_t start_lba;

    uint32_t size;
} __attribute__ ((packed));

void partition_initialize(struct partition *parts, void *mbr)
{
    struct partition_entry *part_table = mbr + PARTITION_OFFSET;

    for (int i = 0; i < 4; ++i)
    {
        parts[i].present = part_table[i].type != 0;
        parts[i].start = part_table[i].start_lba;
        parts[i].size = part_table[i].size;
    }
}
