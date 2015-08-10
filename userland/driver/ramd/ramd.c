#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <zos/print.h>

#include <driver/driver.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct ramd_private {
    size_t size;

    void *data;
};

static void usage(void)
{
    fprintf(stderr, "ramd [OPTS] SIZE NUMBER\n");
    fprintf(stderr, "SIZE: The size of the ram disk\n");
    fprintf(stderr, "NUMBER: The number of the ramdisk"
                    "(ex: 3 ->/dev/ramd3)\n");
    fprintf(stderr, "--daemon: Launch driver as daemon\n");
}

static long int extract_size(const char *size)
{
    char *endptr;
    long int res;

    res = strtol(size, &endptr, 10);

    if (res == 0)
        return 0;

    if (*endptr == 'K')
    {
        res *= 1024;
        ++endptr;
    }
    else if (*endptr == 'M')
    {
        res *= 1024 * 1024;
        ++endptr;
    }

    if (*endptr)
        return 0;

    return res;
}

static int extract_number(const char *number)
{
    char *endptr;
    long int res;

    res = strtol(number, &endptr, 10);

    if (*endptr)
        return -1;

    if (res <= 0)
        return -1;

    if (res > 255)
        return -1;

    return res;
}

static int ram_open(struct driver *driver, int mid, struct req_open *request,
                    ino_t *inode)
{
    (void) request;
    (void) mid;
    (void) driver;

    *inode = 0;

    return 0;
}

static int ram_read(struct driver *driver, int mid, struct req_rdwr *msg,
                    size_t *size_read)
{
    (void) mid;

    struct ramd_private *ram = driver->private;
    size_t to_read = MIN(msg->size, ram->size - msg->off);

    /* End of file */
    if (ram->size <= msg->off)
    {
        *size_read = 0;

        return 0;
    }

    memcpy(msg->data, (char *)ram->data + msg->off, to_read);

    *size_read = to_read;

    return 0;
}

static int ram_write(struct driver *driver, int mid, struct req_rdwr *msg,
                     size_t *size_write)
{
    (void) mid;

    struct ramd_private *ram = driver->private;
    size_t to_write = MIN(msg->size, ram->size - msg->off);

    /* End of file */
    if (ram->size <= msg->off)
    {
        *size_write = 0;

        return 0;
    }

    memcpy((char *)ram->data + msg->off, msg->data, to_write);

    *size_write = to_write;

    return 0;
}

static int ram_close(struct driver *driver, int mid, struct req_close *msg)
{
    (void) driver;
    (void) mid;
    (void) msg;

    return 0;
}

static struct driver_ops ram_ops = {
    .open = ram_open,
    .read = ram_read,
    .write = ram_write,
    .close = ram_close,
};

int main(int argc, char *argv[])
{
    char name[8];
    int number;
    int daemon = 0;
    long int size;
    char *size_str;
    char *number_str;
    struct driver ram_driver;
    struct ramd_private data;

    if (argc != 3 && argc != 4)
    {
        usage();

        return 1;
    }

    if (argc == 3)
    {
        size_str = argv[1];
        number_str = argv[2];
    }

    if (argc == 4 && !strcmp("--daemon", argv[1]))
    {
        size_str = argv[2];
        number_str = argv[3];

        daemon = 1;
    }
    else
    {
        usage();

        return 1;
    }

    size = extract_size(size_str);
    number = extract_number(number_str);

    if (size <= 0)
    {
        fprintf(stderr, "Invalid size format\n");

        return 2;
    }

    if (number < 0)
    {
        fprintf(stderr, "Invalid ram number (1 - 255)\n");

        return 3;
    }

    if (daemon)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            fprintf(stderr, "Cannot daemonize\n");

            return 5;
        }

        if (pid)
            return 0;
    }

    if (!(data.data = malloc(size)))
    {
        uprint("Cannot allocate memory");

        return 1;
    }

    data.size = size;

    ram_driver.private = &data;

    sprintf(name, "ramd%i", number);

    if (driver_create(name, 0600, &ram_ops, &ram_driver) < 0)
    {
        uprint("Cannot create \"ram\" device");

        return 1;
    }

    return driver_loop(&ram_driver);
}
