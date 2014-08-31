#include <unistd.h>

int sleep(size_t seconds)
{
    return usleep(seconds * 1000);
}
