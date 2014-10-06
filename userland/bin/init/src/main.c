#include <unistd.h>
#include <fcntl.h>

#include <zos/print.h>

int main(void)
{
    int timeout = 0;
    int conf_fd;

    uprint("Init is launching");

    while (timeout < 5000)
    {
        conf_fd = open("/etc/init_conf", 0, 0);

        if (conf_fd >= 0)
            break;

        usleep(50);
        timeout += 50;
    }

    if (timeout >= 5000)
    {
        uprint("Init failed to open it's configuration file. Aborting...");

        return 1;
    }

    uprint("Init configuration file open with sucess");

    return 0;
}
