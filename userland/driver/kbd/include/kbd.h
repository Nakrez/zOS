#ifndef KBD_H
# define KBD_H

# include <sys/spinlock.h>

# include <driver/driver.h>

struct kbd {
    int opened;

    int mid;

    struct req_rdwr req;

    spinlock_t lock;

    struct driver driver;
};

#endif /* !KBD_H */
