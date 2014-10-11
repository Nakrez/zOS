#include "file.h"

int ext2fs_open(struct fiu_internal *fiu, struct req_open *req,
                struct resp_open *response)
{
    (void) fiu;
    (void) req;
    (void) response;

    return -1;
}

int ext2fs_close(struct fiu_internal *fiu, struct req_close *req)
{
    (void) fiu;
    (void) req;

    return 0;
}
