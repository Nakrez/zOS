#ifndef DRIVER_DRIVER_H
# define DRIVER_DRIVER_H

# include <zos/vfs.h>

struct driver {
    const char *dev_name;

    int dev_id;

    int running;

    int ops;

    struct driver_ops *dev_ops;
};

struct driver_ops {
    void (*open)(struct driver *, int, struct open_msg *);
    void (*read)(struct driver *, int, struct rdwr_msg *);
    void (*write)(struct driver *, int, struct rdwr_msg *);
    void (*close)(struct driver *, int, struct close_msg *);
};

int driver_create(const char *dev_name, int uid, int gid, int perm,
                  struct driver_ops *dev_ops, struct driver *result);

int driver_loop(struct driver *driver);

#endif /* !DRIVER_DRIVER_H */
