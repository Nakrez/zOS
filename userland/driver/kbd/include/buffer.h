#ifndef KBD_BUFFER_H
# define KBD_BUFFER_H

# include <zos/input.h>

# define BUFFER_MAX_SIZE 255

int buffer_empty(void);
int buffer_full(void);
int buffer_pop(struct input_event *event);
int buffer_push(struct input_event *event);

#endif /* !KBD_BUFFER_H */
