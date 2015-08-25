#ifndef EXT2_FILE_H
# define EXT2_FILE_H

# include <zos/vfs.h>

struct fiu_instance;

/**
 *  \brief  Open a new file
 */
int ext2fs_open(struct fiu_instance *fi, struct req_open *req,
                struct resp_open *response);

/**
 *  \brief  Read a file
 */
int ext2fs_read(struct fiu_instance *fi, struct req_rdwr *req, size_t *size);

/**
 *  \brief  Perform a getdirent()
 */
int ext2fs_getdirent(struct fiu_instance *fi, struct req_getdirent *req,
                     struct dirent *dirent);

/**
 *  \brief  Perform a close()
 */
int ext2fs_close(struct fiu_instance *fi, struct req_close *req);

#endif /* !EXT2_FILE_H */
