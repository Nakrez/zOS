#include <unistd.h>
#include <string.h>

#include <zos/print.h>

#include "tty_ctrl.h"

static int tty_ctrl_open(struct driver *driver, struct req_open *req,
                         ino_t *inode)
{
    int i;
    struct tty_ctrl *ctrl = driver->private;

    for (i = 0; i < ctrl->nb_slave; ++i) {
        if (ctrl->slaves[i].pid == req->pid)
            break;
    }

    /*
     * A tty slave just opened us, we need to remember it because only slaves
     * can read/write from tty device
     */
    if (i != ctrl->nb_slave)
        *inode = req->pid;
    else
        *inode = 0;

    return 0;
}

static int tty_ctrl_read(struct driver *driver, struct req_rdwr *req,
                         size_t *size)
{
    int ret = 0;
    struct tty_ctrl *ctrl = driver->private;

    spinlock_lock(&ctrl->input.lock);

    if (ctrl->input.size) {
        *size = 0;

        while (req->size && ctrl->input.size > 0) {
            *((char *)(req->data++)) = tty_ctrl_input_pop(ctrl);

            --req->size;
            ++(*size);
        }
    } else if (ctrl->slaves[ctrl->nb_slave].slave_id >= 0) {
        /* TODO: EIO */
        ret = -1;
    } else {
        ctrl->slaves[ctrl->nb_slave].slave_id = req->hdr.slave_id;
        memcpy(&ctrl->slaves[ctrl->nb_slave].req, req,
               sizeof (struct req_rdwr));

        spinlock_unlock(&ctrl->input.lock);

        ret = DRV_NORESPONSE;
    }

    spinlock_unlock(&ctrl->input.lock);

    return ret;
}

static int tty_ctrl_write(struct driver *driver, struct req_rdwr *req,
                          size_t *size)
{
    int ret;
    struct tty_ctrl *ctrl = driver->private;

    if (!req->inode)
        return -1;

    /* TODO: Handle multiple tty slave */
    ret = write(ctrl->video_fd, req->data, req->size);
    if (ret < 0)
        return ret;

    *size = ret;

    return 0;
}

static struct driver_ops tty_ctrl_ops = {
    .open = tty_ctrl_open,
    .read = tty_ctrl_read,
    .write = tty_ctrl_write,
};

int tty_ctrl_driver(struct tty_ctrl *ctrl)
{
    int ret;

    ret = driver_create("tty", 0600, &tty_ctrl_ops, &ctrl->driver);
    if (ret < 0) {
        uprint("tty_ctrl: Fail to spawn tty device");
        return ret;
    }

    ctrl->driver.private = ctrl;

    uprint("tty_ctrl: tty device is now ready");

    return driver_loop(&ctrl->driver);
}
