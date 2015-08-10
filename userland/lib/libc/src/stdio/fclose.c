#include <stdio.h>

#include "iobuffer.h"

int fclose(FILE *fp)
{
    if (!fp)
        return -1;

    fflush(fp);

    iob_destroy(fp);

    iob_remove(fp);

    return 0;
}
