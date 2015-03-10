#ifndef FIU_OPTS_H
# define FIU_OPTS_H

# include <stdint.h>

struct fiu_opts {
    mode_t mode;

    uid_t uid;
    gid_t gid;

    int daemon;
    int help;

    char *device;

    char *dir;
};

int fiu_parse_opts(int argc, char **argv, struct fiu_opts *opts);

#endif /* !FIU_OPTS_H */
