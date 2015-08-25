#ifndef EXT2_BLOCK_H
# define EXT2_BLOCK_H

# include <stdint.h>

struct fiu_instance;

/**
 *  \brief  Fetch a block from the device
 *
 *  \param  fi      The file system instance
 *  \param  buffer  The buffer that will be filled with the data
 *  \param  block   The size to fetch (in block)
 *
 *  \return 0: Everything went well
 */
int ext2_block_fetch(struct fiu_instance *fi, void *buffer, uint32_t block);

/**
 *  \brief  Flush a block to the device
 *
 *  \param  fi      The file system instance
 *  \param  buffer  The buffer that will be flushed on the device
 *  \param  block   The size to fetch (in block)
 *
 *  \return 0: Everything went well
 */
int ext2_block_flush(struct fiu_instance *fi, void *buffer, uint32_t block);

#endif /* !EXT2_BLOCK_H */
