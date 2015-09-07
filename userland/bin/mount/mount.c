#include <unistd.h>
#include <stdio.h>
#include <getopt.h>

#include <sys/mount.h>

struct mount_params {
    int help;

    char *fs_type;

    char *mount_pt;

    char *device;
};

static struct option mount_options[] = {
    { "help", no_argument, NULL, 'h' },
    { "type", no_argument, NULL, 't' },
    { 0, 0, 0, 0 },
};

static void mount_help(FILE *out)
{
    fprintf(out, "Usage:\n");
    fprintf(out, "mount [options] <mount_pt>\n");
    fprintf(out, "mount [options] <device> <mount_pt>\n");
    fprintf(out, "\nOptions:\n");
    fprintf(out, "  -h / --help             : Print this help\n");
    fprintf(out, "  -t / --type <fs_type>   : Precise the type of the file"
                 "system\n");
}

static int parse_opts(int argc, char *argv[], struct mount_params *opts)
{
    int c;

    for (;;) {
        int opt_index = 0;
        c = getopt_long(argc, argv, "ht:", mount_options, &opt_index);

        if (c == -1)
            break;

        switch (c)
        {
            case 'h':
                opts->help = 1;
                return 0;
            case 't':
                opts->fs_type= optarg;
                break;
            default:
                return -1;
        }
    }

    if (optind == argc) {
        fprintf(stderr, "No mount point or device specified\n");
        return -1;
    }

    if (!opts->fs_type) {
        fprintf(stderr, "You MUST specify the type of the file system\n");
        return -1;
    }

    switch (argc - optind) {
        case 1:
            opts->mount_pt = argv[optind];
            break;
        case 2:
            opts->device = argv[optind];
            opts->mount_pt = argv[optind + 1];
            break;
        default:
            return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int ret;
    struct mount_params opts;

    opts.help = 0;
    opts.fs_type = NULL;
    opts.mount_pt = "";
    opts.device = "";

    ret = parse_opts(argc, argv, &opts);
    if (ret < 0) {
        mount_help(stderr);
        return 1;
    }

    if (opts.help) {
        mount_help(stdout);
        return 0;
    }

    ret = mount(opts.fs_type, opts.device, opts.mount_pt);
    if (ret < 0) {
        fprintf(stderr, "mount(\"%s\", \"%s\", \"%s\") = %i\n", opts.fs_type,
                opts.device, opts.mount_pt, ret);
        return 1;
    }

    return 0;
}
