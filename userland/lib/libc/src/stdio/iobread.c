#include <unistd.h>

#include "iobuffer.h"

static size_t read_iob(char *buf, size_t size, struct _IO_FILE *stream)
{
    size_t ret = 0;

    while (stream->_io_read_ptr < stream->_io_read_end && size > 0)
    {
        *(buf++) = *(stream->_io_read_ptr++);
        ++ret;
        --size;
    }

    return ret;
}

size_t iob_read(void *ptr, size_t size, struct _IO_FILE *stream)
{
    size_t ret = 0;
    char *buf = ptr;
    int read_ret;

    if (!size)
        return 0;

    /* If there is chars in the buffer take from here */
    if (stream->_io_read_ptr < stream->_io_read_end)
    {
        ret = read_iob(buf, size, stream);

        buf += ret;
        size -= ret;

        if (size == 0)
            return ret;
    }

    /* We asked more than our buffer capacity, directly fill output buffer */
    if (size >= _IO_BUFSIZE_READ)
    {
        read_ret = read(stream->fd, buf, size);

        if (read_ret < 0)
            return ret;

        ret += read_ret;

        return ret;
    }

    /* We need to refresh our buffer */
    stream->_io_read_ptr = stream->_io_read_base;
    stream->_io_read_end = stream->_io_read_base;

    read_ret = read(stream->fd, stream->_io_read_ptr, _IO_BUFSIZE_READ);

    if (read_ret <= 0)
        return ret;

    stream->_io_read_end += read_ret;

    return ret + read_iob(buf, size, stream);
}
