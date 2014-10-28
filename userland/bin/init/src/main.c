#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <zos/print.h>

# define INIT_CONF_TIMEOUT 5000
# define INIT_CONF_WAIT 100
# define INIT_CONF_PATH "/etc/init_conf"
# define INIT_READ_SIZE 4096

/* TODO: FIXME */
static char buf[255];

static int open_init_conf(void)
{
    int fd;
    int timeout = 0;

    while (timeout < INIT_CONF_TIMEOUT)
    {
        if ((fd = open(INIT_CONF_PATH, O_RDONLY, 0)) >= 0)
            return fd;

        timeout += INIT_CONF_WAIT;
        usleep(INIT_CONF_WAIT);
    }

    return -1;
}

static char **split_init_conf(char *config)
{
    int current_size = 0;
    int size = 4;
    char **conf = malloc(sizeof (char *) * size);
    char *line_begin = config;

    while (1)
    {
        if (*config == '\n' || *config == '\0')
        {
            if (current_size == size)
            {
                char **tmp;
                size *= 2;

                if (!(tmp = realloc(conf, sizeof (char *) * size)))
                {
                    free(conf);

                    return NULL;
                }

                conf = tmp;

            }

            conf[current_size++] = line_begin;

            if (*config == '\0')
                break;

            *config = '\0';
            line_begin = config + 1;
        }

        ++config;
    }

    if (current_size == size)
    {
        char **tmp;

        size += 1;

        if (!(tmp = realloc(conf, sizeof (char *) * size)))
        {
            free(conf);

            return NULL;
        }

        conf = tmp;
    }

    conf[current_size] = NULL;

    return conf;
}

static char **read_init_conf(int fd)
{
    int ret;
    char *buf;
    char *tmp;
    struct stat init_stat;

    if (fstat(fd, &init_stat) < 0)
        return NULL;

    if (!(buf = malloc(init_stat.st_size + 1)))
        return NULL;

    tmp = buf;

    while (init_stat.st_size > 0)
    {
        if ((ret = read(fd, tmp, init_stat.st_size)) < 0)
        {
            uprint("TEST");
            free(buf);

            return NULL;
        }

        tmp += ret;
        init_stat.st_size -= ret;
    }

    *tmp = '\0';

    return split_init_conf(buf);
}

int main(void)
{
    int conf_fd = 0;
    char **init_conf = NULL;
    pid_t pid;

    if ((conf_fd = open_init_conf()) < 0)
    {
        uprint("Init: Failed to open it's configuration file.");
        uprint("Init: Canceling boot");

        return 1;
    }

    if (!(init_conf = read_init_conf(conf_fd)))
    {
        uprint("Init: Failed to read it's configuration file.");
        uprint("Init: Canceling boot");

        close(conf_fd);

        return 1;
    }

    close(conf_fd);

    uprint("Init: Configuration read with sucess !");
    uprint("Init: Starting system !");

    for (int i = 0; init_conf[i]; ++i)
    {
        if (!*init_conf[i])
            continue;

        sprintf(buf, "Init: Launching %s", init_conf[i]);

        uprint(buf);

        pid = fork();

        if (pid < 0)
        {
            uprint("Init: fork() failed");

            continue;
        }

        if (pid == 0)
        {
            if (execve(init_conf[i], NULL, NULL) < 0)
                uprint("Init: execve() failed");

            return 1;
        }
    }

    /* We only free 0 because we know all lines are from the same buffer */
    free(init_conf[0]);
    free(init_conf);

    while (1)
        sleep(1);

    return 0;
}
