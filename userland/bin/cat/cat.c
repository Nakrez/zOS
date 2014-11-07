#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

static void usage(void)
{
    write(1, "cat\n", 4);
}

static char buf[4096];

int main(int argc, char *argv[])
{
    int fd;
    int ret;

    if (argc != 2)
        usage();
    else
    {
        if ((fd = open(argv[1], O_RDONLY, 0)) < 0)
        {
            write(STDERR_FILENO, argv[1], strlen(argv[1]));
            write(STDERR_FILENO, ": No such file or directory\n", 28);

            return 1;
        }

        while ((ret = read(fd, buf, 4096)) > 0)
            write(STDOUT_FILENO, buf, ret);

        if (ret < 0)
        {
            write(STDERR_FILENO, "\nread error\n", 12);

            close(fd);

            return 1;
        }

        close(fd);
    }

    return 0;
}
