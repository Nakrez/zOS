#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <thread.h>

#include <zos/print.h>
#include <zos/device.h>

#include "tty.h"

# define TTY_WAIT_CTRL 1000
# define TTY_WAIT_DELAY 100

static int tty_wait_ctrl(void)
{
    int timeout = 0;
    int fd;

    while (timeout < TTY_WAIT_CTRL) {
        fd = open_device("tty", O_RDWR, 0);
        if (fd >= 0)
            return fd;

        timeout += TTY_WAIT_CTRL;
        usleep(TTY_WAIT_CTRL);
    }

    return -1;
}

static int tty_flush_buffer(struct tty *tty, char *dest, size_t max_size)
{
    int size = 0;

    while (max_size) {
        *(dest++) = tty->input.buffer[size];

        if (tty->input.buffer[size] == '\n') {
            --tty->input.nb_line;

            ++size;
            --max_size;

            break;
        }

        ++size;
        --max_size;
    }

    memcpy(tty->input.buffer, tty->input.buffer + size, tty->input.max_size);

    tty->input.size -= size;

    return size;
}

static int tty_read(struct driver *driver, int mid, struct req_rdwr *req,
                    size_t *size)
{
    struct tty *tty = driver->private;

    spinlock_lock(&tty->input.lock);

    if (tty->req.mid != 0) {
        spinlock_unlock(&tty->input.lock);

        /* TODO: EIO */
        return -1;
    } else if (tty->input.nb_line > 0) {
        *size = tty_flush_buffer(tty, req->data, req->size);
    } else {
        memcpy(&tty->req.req, req, sizeof (struct req_rdwr));

        tty->req.mid = mid;

        spinlock_unlock(&tty->input.lock);

        return DRV_NORESPONSE;
    }

    spinlock_unlock(&tty->input.lock);

    return 0;
}

static int tty_write(struct driver *driver, int mid, struct req_rdwr *req,
                     size_t *size)
{
    (void) mid;

    int ret;
    struct tty *tty = driver->private;

    ret = write(tty->tty_ctrl_fd, req->data, req->size);

    if (ret < 0)
        return ret;

    *size = ret;

    return 0;
}

static struct driver_ops tty_ops = {
    .read = tty_read,
    .write = tty_write,
};

static void tty_input_thread(int argc, void *argv[])
{
    (void)argc;

    int ret;
    struct tty *tty = argv[0];

    for (;;) {
        if (tty->input.size == tty->input.max_size) {
            char *tmp = realloc(tty->input.buffer, tty->input.max_size * 2);

            if (!tmp)
                continue;

            tty->input.buffer = tmp;
            tty->input.max_size *= 2;
        }

        ret = read(tty->tty_ctrl_fd, tty->input.buffer + tty->input.size,
                  tty->input.max_size - tty->input.size);
        if (ret < 0)
            continue;

        for (int i = 0; i < ret; ++i) {
            if (tty->input.buffer[tty->input.size] == '\n')
                ++tty->input.nb_line;
        }

        tty->input.size += ret;

        spinlock_lock(&tty->input.lock);

        if (tty->req.mid && tty->input.nb_line > 0) {
            struct resp_rdwr response;

            response.ret = 0;
            response.size = tty_flush_buffer(tty, tty->req.req.data,
                                             tty->req.req.size);

            device_send_response(tty->driver.dev_id, tty->req.mid, &response,
                                 sizeof (struct resp_rdwr));

            tty->req.mid = 0;
        }

        spinlock_unlock(&tty->input.lock);
    }
}

int main(void)
{
    int ret;
    struct tty tty;

    uprint("tty: Initialization");

    /*
     * We have to wait for tty controller because it spawns slaves before
     * initializing tty device
     */
    tty.tty_ctrl_fd = tty_wait_ctrl();
    if (tty.tty_ctrl_fd < 0) {
        uprint("tty: Fail to attach to tty controller");
        return 1;
    }

    tty.input.buffer = malloc(TTY_INPUT_BUFFER_SIZE);
    if (!tty.input.buffer) {
        uprint("tty: Memory exhausted");
        return 1;
    }

    tty.input.size = 0;
    tty.input.max_size = TTY_INPUT_BUFFER_SIZE;
    tty.input.nb_line = 0;

    tty.req.mid = 0;

    spinlock_init(&tty.input.lock);

    uprint("tty: Now attached to tty device");

    ret = driver_create("tty0", 0600, &tty_ops, &tty.driver);
    if (ret < 0) {
        uprint("tty: Fail to spawn tty0 device");
        return 1;
    }

    ret = thread_create(tty_input_thread, 1, &tty);
    if (ret < 0) {
        uprint("tty: Fail to spwan input thread");
        return 1;
    }

    uprint("tty: tty0 device is now ready");

    tty.driver.private = &tty;

    return driver_loop(&tty.driver);
}
