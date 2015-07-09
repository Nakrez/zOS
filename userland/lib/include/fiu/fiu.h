#ifndef LIBFIU_FIU_H
# define LIBFIU_FIU_H

# include <zos/vfs.h>

# include <fiu/opts.h>
# include <fiu/block_cache.h>

struct fiu_internal {
    char *device_name;

    dev_t dev_id;

    int running;

    struct fiu_ops *ops;

    vop_t capabilities;

    struct fiu_cache *block_cache;

    void *private;
};

struct fiu_ops {
    void *(*fill_private)(struct fiu_internal *fiu, struct fiu_opts *opts);
    int (*lookup)(struct fiu_internal *, struct req_lookup *,
                  struct resp_lookup *);
    int (*stat)(struct fiu_internal *, struct req_stat *, struct stat *);
    int (*mount)(struct fiu_internal *, struct req_mount *);
    int (*open)(struct fiu_internal *, struct req_open *, struct resp_open *);
    int (*read)(struct fiu_internal *, struct req_rdwr *, size_t *);
    int (*getdirent)(struct fiu_internal *, struct req_getdirent*,
                     struct dirent *);
    int (*close)(struct fiu_internal *, struct req_close *);
};

int fiu_create(const char *name, int perm, struct fiu_ops *ops,
               struct fiu_internal *fiu);

int fiu_main(int argc, char **argv, struct fiu_ops *ops);

#endif /* !LIBFIU_FIU_H */
