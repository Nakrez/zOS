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
    void (*open)(struct driver *driver, int mid, struct open_msg *);
    void (*read)(struct driver *driver, int mid, struct read_msg *);
    void (*write)(char *buf);
    void (*close)(char *buf);
};

int driver_create(const char *dev_name, int uid, int gid, int perm,
                  struct driver_ops *dev_ops, struct driver *result);

int driver_loop(struct driver *driver);

#endif /* !DRIVER_DRIVER_H */
