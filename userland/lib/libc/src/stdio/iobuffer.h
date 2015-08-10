#ifndef LIBC_STDIO_IOBUFFER_H
# define LIBC_STDIO_IOBUFFER_H

# include <sys/types.h>
# include <sys/spinlock.h>

# define _IO_BUFSIZE_READ 1024
# define _IO_BUFSIZE_WRITE 1024

struct _IO_FILE {
    int fd;
    off_t offset;

    int flags;

    char *_io_read_ptr;
    char *_io_read_end;
    char *_io_read_base;

    char *_io_write_ptr;
    char *_io_write_end;
    char *_io_write_base;

    struct _IO_FILE *next;
    struct _IO_FILE *prev;
};

extern struct _IO_FILE *_IO_files;
extern spinlock_t _IO_lock;

struct _IO_FILE *iob_create(int fd, int flags);

void iob_add(struct _IO_FILE *file);
size_t iob_read(void *ptr, size_t size, struct _IO_FILE *stream);
int iob_putc(int c, struct _IO_FILE *stream);
int iob_puti(struct _IO_FILE *stream, int num, unsigned int base);
int iob_putu(struct _IO_FILE *stream, unsigned int num, unsigned int base);
int iob_flush(struct _IO_FILE *file);
void iob_destroy(struct _IO_FILE *file);
void iob_remove(struct _IO_FILE *file);

#endif /* !LIBC_STDIO_IOBUFFER_H */
