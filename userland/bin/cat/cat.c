#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <sys/stat.h>

# define BUFFER_SIZE 4096

static void usage(void)
{
    fprintf(stderr, "cat [FILE]...\n");
}

static char buf[BUFFER_SIZE];

static void cat_file(const char *filename)
{
    FILE *file;
    size_t ret;
    struct stat s;

    if (!(file = fopen(filename, "r")))
    {
        fprintf(stderr, "%s: No such file or directoy\n", filename);

        return;
    }

    if (stat(filename, &s) < 0)
    {
        fprintf(stderr, "%s: Cannot stat\n", filename);

        return;
    }

    if (S_ISDIR(s.st_mode))
    {
        fprintf(stderr, "%s: Is a directory\n", filename);
    }

    while ((ret = fread(buf, BUFFER_SIZE - 1, 1, file)) != 0)
    {
        buf[ret] = 0;
        fputs(buf, stdout);
    }

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
