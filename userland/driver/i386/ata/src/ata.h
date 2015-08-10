#ifndef ATA_H
# define ATA_H

# include <stdint.h>

# include "ide.h"

# define ATA_OP_READ (1 << 0)
# define ATA_OP_WRITE (1 << 1)

# define ATA_SECTOR_SIZE 512

int ata_rdwr(struct ide_device *device, int op, uint64_t lba, void *buf,
             size_t sec_size, size_t sec_count);

#endif /* !ATA_H */
