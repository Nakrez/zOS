#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <fiu/opts.h>

static struct option fiu_options[] = {
    { "help", no_argument, NULL, 'h' },
    { "daemon", no_argument, NULL, 'd' },
    { 0, 0, 0, 0 },
};

static void init_opts(struct fiu_opts *opts)
{
    /* Default mode */
    opts->mode = 0644;

    /* TODO: getuid() and getgid() */
    opts->uid = 0;
    opts->gid = 0;

    opts->daemon = 0;
    opts->help = 0;

    opts->device = NULL;
    opts->dir = NULL;
}

int fiu_parse_opts(int argc, char **argv, struct fiu_opts *opts)
{
    int c;

    init_opts(opts);

    while (1)
    {
        int opt_index = 0;

        c = getopt_long(argc, argv, "hd", fiu_options, &opt_index);

        if (c == -1)
            break;

        switch (c)
        {
            case 'h':
                opts->help = 1;
                break;
            case 'd':
                opts->daemon = 1;
                break;
            default:
                return -1;
        }
    }

    return 0;
}
