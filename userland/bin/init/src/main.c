#include <unistd.h>

#include <zos/print.h>

int main(void)
{
    uprint("Init is launching");

    while (1)
        sleep(1);

    return 0;
}
