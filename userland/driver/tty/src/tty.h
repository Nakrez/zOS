#ifndef TTY_H
# define TTY_H

# include <sys/spinlock.h>

# include <driver/driver.h>

# define TTY_INPUT_BUFFER_SIZE 256

struct tty {
    int tty_ctrl_fd;

    struct {
        struct req_rdwr req;
        int mid;
    } req;

    struct {
        char *buffer;

        int size;
        int max_size;
        int nb_line;

        spinlock_t lock;
    } input;

    struct driver driver;
};

#endif /* !TTY_H */
