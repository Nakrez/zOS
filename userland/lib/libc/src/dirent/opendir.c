#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/stat.h>

DIR *opendir(const char *dirname)
{
    DIR *d;
    struct stat s;

    if (!(d = malloc(sizeof (DIR))))
        return NULL;

    if ((d->fd = open(dirname, O_RDONLY, 0)) < 0)
    {
        free(d);

        return NULL;
    }

    if (fstat(d->fd, &s) < 0 || !S_ISDIR(s.st_mode))
    {
        close(d->fd);

        free(d);

        return NULL;
    }

    d->index = 0;

    return d;
}
