#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/mount.h>

#include <zos/print.h>

#include "config.h"
#include "exec.h"

# define INIT_CONF_PATH "/etc/init_conf"

int mount_devfs(void)
{
    int ret;
    struct stat s;

    ret = stat("/dev", &s);
    if (ret < 0)
        return -1;

    if (!S_ISDIR(s.st_mode))
        return -1;

    return mount("devfs", "", "/dev");
}

int main(void)
{
    int conf_fd;
    int ret;
    struct init_conf *config;

    if (getpid() != 1) {
        uprint("Init: Cannot launch init twice");
        return 1;
    }

    do {
        ret = mount("ext2", "ata-disk0", "/");
        if (ret < 0)
            usleep(500);
    } while (ret < 0);

    ret = mount_devfs();
    if (ret < 0) {
        uprint("Init: Failed to mount devfs");
        return 1;
    }

    uprint("Init: devfs mounted with success on /dev");

    config = init_conf_create();
    if (!config) {
        uprint("Init: Not enough memory");
        uprint("Init: Canceling boot");
        return 1;
    }

    conf_fd = open(INIT_CONF_PATH, O_RDONLY, 0);
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
