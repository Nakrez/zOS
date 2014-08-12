#include <unistd.h>

int main(void)
{
    char message[4];

    message[0] = 'V';
    message[1] = 'F';
    message[2] = 'S';
    message[3] = 0;

    while (1)
    {
        __asm__ __volatile__("mov %0, %%ebx\n"
                             "mov $1, %%eax\n"
                             "int $0x80\n"
                             :
                             : "r" (message)
                             : "memory");

        sleep(1);
    }

    return 0;
}
