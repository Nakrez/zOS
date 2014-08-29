#ifndef DRIVER_DRIVER_H
# define DRIVER_DRIVER_H

struct driver_ops {
    void (*open)(void);
    void (*read)(void);
    void (*write)(void);
    void (*close)(void);
};

struct driver {
    const char *dev_name;

    int dev_id;

    int running;

    struct driver_ops *dev_ops;
};

int driver_create(const char *dev_name, int uid, int gid, int perm,
                  struct driver_ops *dev_ops, struct driver *result);

#endif /* !DRIVER_DRIVER_H */
