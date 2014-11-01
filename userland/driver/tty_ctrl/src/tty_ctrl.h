#ifndef TTY_CTRL_H
# define TTY_CTRL_H

# include <sys/types.h>
# include <sys/spinlock.h>

# include <driver/driver.h>

# define DRIVER_WAIT_TIME 1000
# define DRIVER_RETRY_TIME 100

# define TTY_XRES 1024
# define TTY_YRES 768

/* FIXME: Use font.h */
# define FONT_HEIGHT 16
# define FONT_WIDTH 8

# define TTY_INPUT_BUFFER_SIZE 256

struct tty_slave {
    pid_t pid;

    int mid_req;

    struct req_rdwr req;
};

struct tty_ctrl {
    int video_fd;
    int kbd_fd;

    int nb_slave;
    int current_slave;

    struct tty_slave *slaves;

    struct driver driver;

    struct {
        char buffer[TTY_INPUT_BUFFER_SIZE];

        int start;
        int end;

        int size;

        int shift;
        int ctrl;

        spinlock_t lock;
    } input;
};

extern struct tty_ctrl controller;

struct tty_ctrl *tty_ctrl_create(void);
int tty_ctrl_initialize(struct tty_ctrl *ctrl);
int tty_ctrl_driver(struct tty_ctrl *ctrl);
void tty_ctrl_destroy(struct tty_ctrl *ctrl);
char tty_ctrl_input_pop(struct tty_ctrl *ctrl);

#endif /* !TTY_CTRL_H */
