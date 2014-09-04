#include <unistd.h>
#include <fcntl.h>

#include <zos/print.h>

int main(void)
{
    uprint("Init is launching");

    usleep(200);

    while (1)
        sleep(1);

    return 0;
}
