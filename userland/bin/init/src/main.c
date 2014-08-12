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
        for (volatile int i = 0; i < 10000000; ++i)
            ;

            __asm__ __volatile__("mov %0, %%ebx\n"
                                 "mov $1, %%eax\n"
                                 "int $0x80\n"
                                 :
                                 : "r" (message)
                                 : "memory");
    }

    return 0;
}
