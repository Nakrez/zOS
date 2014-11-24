#include <unistd.h>
#include <stdio.h>

#include "iobuffer.h"

struct _IO_FILE *_IO_files = NULL;
spinlock_t _IO_lock = SPINLOCK_INIT;

FILE *stdin;
FILE *stdout;
FILE *stderr;
