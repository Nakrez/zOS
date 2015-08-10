#ifndef EXT2_FILE_H
# define EXT2_FILE_H

# include <zos/vfs.h>

# include <fiu/fiu.h>

int ext2fs_open(struct fiu_internal *fiu, struct req_open *req,
                struct resp_open *response);
int ext2fs_read(struct fiu_internal *fiu, struct req_rdwr *req, size_t *size);
int ext2fs_getdirent(struct fiu_internal *fiu, struct req_getdirent *req,
                     struct dirent *dirent);
int ext2fs_close(struct fiu_internal *fiu, struct req_close *req);

#endif /* !EXT2_FILE_H */
