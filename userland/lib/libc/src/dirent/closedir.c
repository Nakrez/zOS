#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

int closedir(DIR *d)
{
    int ret;

    if (!d)
        return 0;

    ret = close(d->fd);

    free(d);

    return ret;
}
