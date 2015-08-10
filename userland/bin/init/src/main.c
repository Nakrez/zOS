#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <zos/print.h>

#include "config.h"
#include "exec.h"

# define INIT_CONF_TIMEOUT 20000
# define INIT_CONF_WAIT 100
# define INIT_CONF_PATH "/etc/init_conf"

static int open_file_timeout(const char *file, int wait, int timeout)
{
    int fd;
    int timeout_current = 0;

    while (timeout_current < timeout) {
        fd = open(file, O_RDONLY, 0);
        if (fd >= 0)
            return fd;

        timeout_current += wait;
        usleep(wait);
    }

    return -1;
}

int main(void)
{
    int conf_fd;
    struct init_conf *config;

    if (getpid() != 1) {
        uprint("Init: Cannot launch init twice");
        return 1;
    }

    config = init_conf_create();
    if (!config) {
        uprint("Init: Not enough memory");
        uprint("Init: Canceling boot");
        return 1;
    }

    conf_fd = open_file_timeout(INIT_CONF_PATH, INIT_CONF_WAIT,
                                INIT_CONF_TIMEOUT);
    if (conf_fd < 0) {
        uprint("Init: Failed to open it's configuration file.");
        uprint("Init: Canceling boot");
        init_conf_destroy(config);
        return 1;
    }

    if (init_conf_parse(conf_fd, config) < 0) {
        uprint("Init: Failed to read it's configuration file.");
        uprint("Init: Canceling boot");
        init_conf_destroy(config);
        close(conf_fd);
        return 1;
    }

    close(conf_fd);

    uprint("Init: Configuration read with sucess !");
    uprint("Init: Starting system !");

    init_conf_execute(config);

    init_conf_destroy(config);

    while (1)
        sleep(100);

    return 0;
}
