#include <zos/print.h>

#include <driver/driver.h>

static int null_open(struct driver *driver, int mid, struct req_open *request,
                     ino_t *inode)
{
    (void) request;
    (void) mid;
    (void) driver;

    *inode = 0;

    return 0;
}

static int null_read(struct driver *driver, int mid, struct req_rdwr *msg,
                     size_t *size_read)
{
    (void) driver;
    (void) mid;
    (void) msg;

    *size_read = 0;

    return 0;
}

static int null_write(struct driver *driver, int mid, struct req_rdwr *msg,
                      size_t *size_write)
{
    (void) driver;
    (void) mid;

    *size_write = msg->size;

    return 0;
}

static int null_close(struct driver *driver, int mid, struct req_close *msg)
{
    (void) driver;
    (void) mid;
    (void) msg;

    return 0;
}

static struct driver_ops null_ops = {
    .open = null_open,
    .read = null_read,
    .write = null_write,
    .close = null_close,
};

int main(void)
{
    struct driver null_driver;

    if (driver_create("null", 0666, &null_ops, &null_driver) < 0)
    {
        uprint("Cannot create \"null\" device");

        return 1;
    }

    return driver_loop(&null_driver);
}
