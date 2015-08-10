#include <zos/print.h>

#include "tty_ctrl.h"

int main(void)
{
    int ret;
    struct tty_ctrl *ctrl;

    uprint("tty_ctrl: Initialization");

    if (!(ctrl = tty_ctrl_create()))
    {
        uprint("tty_ctrl: Memory exhausted");

        return 1;
    }

    if (tty_ctrl_initialize(ctrl) < 0)
    {
        uprint("tty_ctrl: Fail to initialize tty controller");

        tty_ctrl_destroy(ctrl);

        return 1;
    }

    uprint("tty_ctrl: Ready");

    ret = tty_ctrl_driver(ctrl);

    tty_ctrl_destroy(ctrl);

    return ret;
}
