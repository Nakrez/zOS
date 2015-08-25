#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <thread.h>

#include <sys/ioctl.h>

#include <zos/video.h>
#include <zos/input.h>
#include <zos/print.h>
#include <zos/device.h>

#include "tty_ctrl.h"
#include "keymap.h"

struct tty_ctrl *tty_ctrl_create(void)
{
    struct tty_ctrl *ctrl;

    ctrl = malloc(sizeof (struct tty_ctrl));
    if (!ctrl)
        return NULL;

    ctrl->video_fd = -1;
    ctrl->kbd_fd = -1;

    ctrl->nb_slave = 0;
    ctrl->current_slave = 0;

    ctrl->slaves = NULL;

    return ctrl;
}

static int open_io_driver(const char *driver, int flags)
{
    int timeout = 0;
    int fd;

    while (timeout < DRIVER_WAIT_TIME) {
        fd = open_device(driver, flags, 0);
        if (fd < 0) {
            usleep(DRIVER_RETRY_TIME);
            timeout += DRIVER_RETRY_TIME;
            continue;
        }

        return fd;
    }

    return -1;
}

static int init_video(struct tty_ctrl *ctrl)
{
    int ret;
    int req;

    req = TTY_XRES;

    ret = ioctl(ctrl->video_fd, VIDEO_XRES_SET, &req);
    if (ret < 0)
        return ret;

    req = TTY_YRES;

    ret = ioctl(ctrl->video_fd, VIDEO_YRES_SET, &req);
    if (ret < 0)
        return ret;

    req = 8;

    ret = ioctl(ctrl->video_fd, VIDEO_BPP_SET, &req);
    if (ret < 0)
        return ret;

    ret = ioctl(ctrl->video_fd, VIDEO_ENABLE, 0);
    if (ret < 0)
        return ret;

    return 0;
}

static int init_slaves(struct tty_ctrl *ctrl, int slaves)
{
    pid_t pid;

    ctrl->nb_slave = 0;

    ctrl->slaves = malloc(slaves * sizeof (struct tty_slave));
    if (!ctrl->slaves)
        /* XXX: ENOMEM */
        return -1;

    for (int i = 0; i < slaves; ++i) {
        pid = fork();

        if (pid < 0)
            continue;

        if (!pid) {
            char *argv[] = { "/bin/tty", NULL };

            execv("/bin/tty", argv);

            uprint("tty_ctrl: Fail to spawn tty slave");

            exit(1);
        }

        ctrl->slaves[ctrl->nb_slave].slave_id = -1;
        ctrl->slaves[ctrl->nb_slave].pid = pid;
        ++ctrl->nb_slave;
    }

    if (ctrl->nb_slave == 0)
        return -1;

    return 0;
}

char tty_ctrl_input_pop(struct tty_ctrl *ctrl)
{
    char c;

    if (ctrl->input.size == 0)
        return -1;

    c = ctrl->input.buffer[ctrl->input.start++];

    if (ctrl->input.start == TTY_INPUT_BUFFER_SIZE)
        ctrl->input.start = 0;

    --ctrl->input.size;

    return c;
}

static void tty_ctrl_input_push(struct tty_ctrl *ctrl, char c)
{
    spinlock_lock(&ctrl->input.lock);

    /* TODO: Remove oldest char and replace it by the new one */
    if (ctrl->input.size == TTY_INPUT_BUFFER_SIZE) {
        spinlock_unlock(&ctrl->input.lock);
        return;
    }

    ctrl->input.buffer[ctrl->input.end++] = c;

    if (ctrl->input.end == TTY_INPUT_BUFFER_SIZE)
        ctrl->input.end = 0;

    ++ctrl->input.size;

    spinlock_unlock(&ctrl->input.lock);

    write(ctrl->video_fd, &c, 1);
}

static void tty_ctrl_input_thread(int argc, void *argv[])
{
    (void) argc;

    int ret;
    struct input_event event;
    struct tty_ctrl *ctrl = argv[0];

    for (;;) {
        ret = read(ctrl->kbd_fd, &event, sizeof (struct input_event));
        if (ret < 0)
            continue;

        if (event.value == EV_KEY_PRESSED)
        {
            switch (event.code) {
                case KEY_LEFTSHIFT:
                case KEY_RIGHTSHIFT:
                    ctrl->input.shift = 1;
                    break;
                case KEY_LEFTCTRL:
                    ctrl->input.ctrl = 1;
                    break;
                default:
                    if (ctrl->input.shift)
                        tty_ctrl_input_push(ctrl, keymap[event.code][2]);
                    else if (ctrl->input.ctrl)
                        tty_ctrl_input_push(ctrl, keymap[event.code][1]);
                    else
                        tty_ctrl_input_push(ctrl, keymap[event.code][0]);
                    break;
            }
        } else if (event.value == EV_KEY_RELEASED) {
            switch (event.code) {
                case KEY_LEFTSHIFT:
                case KEY_RIGHTSHIFT:
                    ctrl->input.shift = 0;
                    break;
                case KEY_LEFTCTRL:
                    ctrl->input.ctrl = 0;
                    break;
            }
        }

        spinlock_lock(&ctrl->input.lock);

        if (ctrl->slaves[ctrl->nb_slave].slave_id >= 0) {
            struct resp_rdwr resp;
            struct req_rdwr *req = &ctrl->slaves[ctrl->nb_slave].req;

            resp.size = 0;
            resp.ret = 0;

            while (req->size && ctrl->input.size > 0) {
                *((char *)(req->data++)) = tty_ctrl_input_pop(ctrl);
                --req->size;
                ++resp.size;
            }

            resp.hdr.slave_id = ctrl->slaves[ctrl->nb_slave].slave_id;

            write(ctrl->driver.channel_fd, &resp, sizeof (resp));

            ctrl->slaves[ctrl->nb_slave].slave_id = -1;
        }

        spinlock_unlock(&ctrl->input.lock);
    }
}

static int init_input_thread(struct tty_ctrl *ctrl)
{
    int tid;

    ctrl->input.start = 0;
    ctrl->input.end = 0;
    ctrl->input.size = 0;
    ctrl->input.shift = 0;
    ctrl->input.ctrl = 0;
    spinlock_init(&ctrl->input.lock);

    tid = thread_create(tty_ctrl_input_thread, 1, ctrl);
    if (tid < 0)
        return -1;

    return 0;
}

int tty_ctrl_initialize(struct tty_ctrl *ctrl)
{
    int ret;

    ctrl->kbd_fd = open_io_driver("kbd", O_RDONLY);
    if (ctrl->kbd_fd < 0)
        return ctrl->video_fd;

    ctrl->video_fd = open_io_driver("video", O_WRONLY);
    if (ctrl->video_fd < 0)
        return ctrl->video_fd;

    ret = init_slaves(ctrl, 1);
    if (ret < 0)
        return ret;

    ret = init_video(ctrl);
    if (ret < 0)
        return ret;

    ret = init_input_thread(ctrl);
    if (ret < 0)
        return ret;

    return 0;
}

void tty_ctrl_destroy(struct tty_ctrl *ctrl)
{
    if (ctrl->video_fd >= 0)
        close(ctrl->video_fd);

    if (ctrl->kbd_fd >= 0)
        close(ctrl->kbd_fd);

    free(ctrl->slaves);
    free(ctrl);
}
