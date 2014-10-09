#ifndef EXT2_BLOCK_H
# define EXT2_BLOCK_H

# include <stdint.h>

# include <fiu/fiu.h>

int ext2_block_fetch(struct fiu_internal *fiu, void *buffer, uint32_t block);
int ext2_block_flush(struct fiu_internal *fiu, void *buffer, uint32_t block);

#endif /* !EXT2_BLOCK_H */
