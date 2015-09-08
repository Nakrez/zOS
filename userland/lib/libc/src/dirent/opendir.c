#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/stat.h>

DIR *opendir(const char *dirname)
{
    DIR *d;
    int ret;
    struct stat s;

    d = malloc(sizeof (DIR));
    if (!d)
        return NULL;

    d->fd = open(dirname, O_RDONLY, 0);
    if (d->fd < 0)
        goto error;

    ret = fstat(d->fd, &s);
    if (ret < 0)
        goto error_stat;

    if (!S_ISDIR(s.st_mode))
        goto error_stat;

    d->index = 0;

    return d;

error_stat:
    close(d->fd);
error:
    free(d);
    return NULL;
}
