#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int ret;
    int tty_fd;
    char buf[256];

    /* TODO: Let init handle it */
    while (1)
    {
        if ((tty_fd = open("/dev/tty0", O_RDWR, 0)) < 0)
        {
            usleep(100);

            continue;
        }

        break;
    }

    while (1)
    {
        write(tty_fd, "root@zOS $ ", 11);

        ret = read(tty_fd, buf, 256);

        if (ret < 0)
        {
            write(tty_fd, "\nread failed\n", 13);
            sleep(1);
        }
        else
            write(tty_fd, buf, ret);
    }

    return 0;
}
