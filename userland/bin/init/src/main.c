#include <unistd.h>

int main(void)
{
    char message[5];

    message[0] = 'I';
    message[1] = 'N';
    message[2] = 'I';
    message[3] = 'T';
    message[4] = 0;

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
