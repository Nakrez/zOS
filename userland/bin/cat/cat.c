#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

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

    if (!(file = fopen(filename, "r")))
    {
        fprintf(stderr, "%s: No such file or directoy\n", filename);

        return;
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
