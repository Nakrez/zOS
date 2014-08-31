#ifndef I386_KEYMAP_H
# define I386_KEYMAP_H

# include <stdint.h>

# include <zos/input.h>

void keymap_scan_to_input_event(uint8_t scan, struct input_event *event);

#endif /* !I386_KEYMAP_H */
