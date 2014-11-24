#include <unistd.h>
#include <stdio.h>

#include <sys/stat.h>

static void usage(void)
{
    fprintf(stderr, "stat FILE...\n");
}

static void stat_file(const char *filename)
{
    struct stat s;

    if (stat(filename, &s) < 0)
    {
        fprintf(stderr, "%s: Cannot stat\n", filename);

        return;
    }

    printf("device  %i\n", s.st_dev);
    printf("inode   %u\n", s.st_ino);
    printf("mode    %o\n", s.st_mode);
    printf("nlink   %u\n", s.st_nlink);
    printf("uid     %u\n", s.st_uid);
    printf("gid     %u\n", s.st_gid);
    printf("rdev    %i\n", s.st_rdev);
    printf("size    %i\n", s.st_size);
    printf("atime   %u\n", s.st_atime);
    printf("mtime   %u\n", s.st_mtime);
    printf("ctime   %u\n", s.st_ctime);
    printf("blksize %i\n", s.st_blksize);
    printf("blocks  %u\n", s.st_blocks);
    printf("link   \n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage();
    else
    {
        for (int i = 1; i < argc; ++i)
            stat_file(argv[i]);
    }

    return 0;
}
