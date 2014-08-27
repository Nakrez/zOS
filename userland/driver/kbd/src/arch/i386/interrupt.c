#include <stdint.h>
#include <zos/interrupt.h>

#include <sys/io.h>

#include <zos/print.h>

#include <buffer.h>

#include <arch/interrupt.h>
#include <arch/keymap.h>

void interrupt_thread(void *arg)
{
    (void) arg;

    int res;
    uint8_t key;
    struct input_event event;

    res = interrupt_register(KEYBOARD_INTERRUPT);

    /* Error while registering the interrupt */
    if (res != 0)
    {
        uprint("Unable to reserve keyboard interrupt...");
        return;
    }

    uprint("Keyboard driver is now ready to receive interrupts");

    while (1)
    {
        res = interrupt_listen(KEYBOARD_INTERRUPT);

        if (res != KEYBOARD_INTERRUPT)
            continue;

        do
        {
            key = inb(KEYBOARD_CMD);
        } while (!(key & 0x01));

        key = inb(KEYBOARD_DATA);

        keymap_scan_to_input_event(key, &event);

        if (event.code == KEY_RESERVED)
            continue;

        buffer_push(&event);
    }
}
