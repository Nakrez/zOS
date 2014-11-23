#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

static void usage(void)
{
    write(STDERR_FILENO, "cat [FILE]...\n", 14);
}

static char buf[4096];

static void cat_file(const char *filename)
{
    FILE *file;
    size_t ret;

    if (!(file = fopen(filename, "r")))
    {
        write(STDERR_FILENO, filename, strlen(filename));
        write(STDERR_FILENO, ": No such file or directory\n", 28);

        return;
    }

    while ((ret = fread(buf, 4096, 1, file)) != 0)
        write(STDOUT_FILENO, buf, ret);

    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage();
    else
    {
        for (int i = 1; i < argc; ++i)
            cat_file(argv[i]);
    }

    return 0;
}
