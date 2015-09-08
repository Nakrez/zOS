#include <stdio.h>
#include <dirent.h>

void print_dir(const char *dirname)
{
    DIR *d;
    struct dirent *dp;

    d = opendir(dirname);
    if (!d) {
        fprintf(stderr, "ls: Cannot open %s\n", dirname);
        return;
    }

    for (;;) {
        dp = readdir(d);
        if (!dp)
            break;

        fprintf(stdout, "%s\n", dp->d_name);
    }

    closedir(d);
}

void usage(void)
{
    fprintf(stderr, "ls [FILES]...\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        usage();
        return 1;
    }

    for (int i = 1; i < argc; ++i)
        print_dir(argv[i]);

    return 0;
}
