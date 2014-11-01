#include <stdint.h>
#include <string.h>

#include <sys/io.h>

#include <zos/interrupt.h>
#include <zos/device.h>
#include <zos/print.h>

#include <buffer.h>

#include <arch/interrupt.h>
#include <arch/keymap.h>

#include <kbd.h>

void interrupt_thread(void *arg)
{
    int res;
    uint8_t key;
    struct input_event event;
    struct kbd *kbd = arg;

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

        spinlock_lock(&kbd->lock);

        buffer_push(&event);

        if (kbd->mid)
        {
            struct resp_rdwr response;

            response.ret = 0;
            response.size = sizeof (struct input_event);

            buffer_pop(&event);

            memcpy(kbd->req.data, &event, sizeof (struct input_event));

            device_send_response(kbd->driver.dev_id, kbd->mid, &response,
                                 sizeof (struct resp_rdwr));

            kbd->mid = 0;
        }

        spinlock_unlock(&kbd->lock);
    }
}
