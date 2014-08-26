#include <string.h>

#include <buffer.h>

static struct input_event buffer[BUFFER_MAX_SIZE + 1];
static int begin = 0;
static int end = 0;

int buffer_empty(void)
{
    return begin == end;
}

int buffer_full(void)
{
    return (end == BUFFER_MAX_SIZE && begin == 0) ||
           (begin - end == 1);
}

int buffer_pop(struct input_event *event)
{
    if (buffer_empty())
        return 1;

    event->type = buffer[begin].type;
    event->code = buffer[begin].code;
    event->value = buffer[begin].value;

    ++begin;

    if (begin > BUFFER_MAX_SIZE)
        begin = 0;

    return 0;
}

int buffer_push(struct input_event *event)
{
    if (buffer_full())
        return 1;

    buffer[end].type = event->type;
    buffer[end].code = event->code;
    buffer[end].value = event->value;

    ++end;

    if (end > BUFFER_MAX_SIZE)
        end = 0;

    return 0;
}
