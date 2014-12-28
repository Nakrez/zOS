#include <stdlib.h>
#include <dirent.h>

struct dirent *readdir(DIR *d)
{
    int ret;

    if (!d)
        return NULL;

    ret = getdirent(d->fd, &d->dp, d->index);

    if (ret < 0 || ret == DIR_END)
        return NULL;

    ++d->index;

    return &d->dp;
}
