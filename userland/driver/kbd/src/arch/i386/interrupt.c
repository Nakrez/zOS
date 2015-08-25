#include <unistd.h>
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

void interrupt_thread(int argc, void *argv[])
{
    (void)argc;

    int ret;
    uint8_t key;
    struct input_event event;
    struct kbd *kbd = argv[0];

    ret = interrupt_register(KEYBOARD_INTERRUPT);
    if (ret < 0) {
        uprint("Unable to reserve keyboard interrupt...");
        return;
    }

    uprint("Keyboard driver is now ready to receive interrupts");

    for (;;) {
        ret = interrupt_listen(KEYBOARD_INTERRUPT);
        if (ret != KEYBOARD_INTERRUPT)
            continue;

        do {
            key = inb(KEYBOARD_CMD);
        } while (!(key & 0x01));

        key = inb(KEYBOARD_DATA);

        keymap_scan_to_input_event(key, &event);

        if (event.code == KEY_RESERVED)
            continue;

        spinlock_lock(&kbd->lock);

        buffer_push(&event);

        if (kbd->slave >= 0) {
            struct resp_rdwr resp;

            resp.ret = 0;
            resp.size = sizeof (struct input_event);

            resp.hdr.slave_id = kbd->slave;

            buffer_pop(&event);

            memcpy(kbd->req.data, &event, sizeof (struct input_event));

            write(kbd->driver.channel_fd, &resp, sizeof (resp));

            kbd->slave = -1;
        }

        spinlock_unlock(&kbd->lock);
    }
}
