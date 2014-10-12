#include "file.h"
#include "inode_cache.h"

int ext2fs_open(struct fiu_internal *fiu, struct req_open *req,
                struct resp_open *response)
{
    struct ext2fs *ext2;
    struct ext2_inode *inode;

    ext2 = fiu->private;

    if (!(inode = ext2_icache_request(ext2, req->inode)))
        return -1;

    if (inode->type_perm & EXT2_TYPE_DIRECTORY)
        return -1;

    response->inode = req->inode;

    return 0;
}

int ext2fs_close(struct fiu_internal *fiu, struct req_close *req)
{
    (void) fiu;
    (void) req;

    return 0;
}
