#include <unistd.h>
#include <fcntl.h>

#include <zos/print.h>

int main(void)
{
    int fd;
    char buf[5];

    uprint("Init is launching");

    usleep(200);

    fd = open("/dev/kbd", 0, 0);

    if (fd < 0)
        uprint("Open failed");

    uprint("File opened");

    if (read(fd, buf, 5) < 0)
        uprint("Read failed");

    uprint("Response: ");

    uprint(buf);

    while (1)
        sleep(1);

    return 0;
}
