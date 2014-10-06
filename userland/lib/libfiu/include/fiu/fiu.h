#ifndef LIBFIU_FIU_H
# define LIBFIU_FIU_H

# include <zos/vfs.h>

struct fiu_internal {
    int dev_id;

    int running;

    struct fiu_ops *ops;

    int capabilities;
};

struct fiu_ops {
    void (*root_remount)(struct req_root_remount *);
    int (*lookup)(struct req_lookup *, struct resp_lookup *);
    int (*open)(struct req_open *, struct resp_open *);
    int (*close)(struct req_close *);
};

int fiu_create(const char *name, uint16_t uid, uint16_t gid, int perm,
               struct fiu_ops *ops, struct fiu_internal *fiu);

int fiu_main(struct fiu_internal *fiu, const char *mount_path);

#endif /* !LIBFIU_FIU_H */
