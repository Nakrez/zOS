#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <zos/print.h>

# define INIT_CONF_TIMEOUT 20000
# define INIT_CONF_WAIT 100
# define INIT_CONF_PATH "/etc/init_conf"
# define INIT_READ_SIZE 4096

/* TODO: FIXME */
static char buf[255];

static int open_file_timeout(const char *file, int wait, int timeout)
{
    int fd;
    int timeout_current = 0;

    while (timeout_current < timeout)
    {
        if ((fd = open(file, O_RDONLY, 0)) >= 0)
            return fd;

        timeout_current += wait;
        usleep(wait);
    }

    return -1;
}

static char **split_init_conf(char *config)
{
    int current_size = 0;
    int size = 4;
    char **conf = malloc(sizeof (char *) * size);
    char *line_begin = config;

    if (!conf)
        return NULL;

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
            free(buf);

            return NULL;
        }

        tmp += ret;
        init_stat.st_size -= ret;
    }

    *tmp = '\0';

    return split_init_conf(buf);
}

static void exec_conf_line(char *line)
{
    pid_t pid;
    char *bin;
    char *rest;
    int in = -1;
    int out = -1;
    int err = -1;

    if (!line || !(*line))
        return;

    bin = strtok_r(line, ",", &rest);

    if (*rest)
    {
        in = open_file_timeout(rest, INIT_CONF_WAIT,
                               INIT_CONF_TIMEOUT);
        if (in == STDIN_FILENO)
        {
            out = dup2(in, STDOUT_FILENO);
            err = dup2(in, STDERR_FILENO);
        }
        else if (in >= 0)
            close(in);
    }

    sprintf(buf, "Init: Launching %s", line);

    uprint(buf);

    pid = fork();

    if (pid < 0)
    {
        uprint("Init: fork() failed");

        return;
    }

    if (pid == 0)
    {
        char *argv[] = { bin, NULL };

        if (execv(bin, argv) < 0)
            uprint("Init: execve() failed");

        exit(1);
    }

    if (*rest)
    {
        if (in == STDIN_FILENO)
            close(in);
        if (out >= 0)
            close(out);
        if (err >= 0)
            close(err);
    }
}

int main(void)
{
    int conf_fd = 0;
    char **init_conf = NULL;

    conf_fd = open_file_timeout(INIT_CONF_PATH, INIT_CONF_WAIT,
                                INIT_CONF_TIMEOUT);

    if (conf_fd < 0)
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
        exec_conf_line(init_conf[i]);

    /* We only free 0 because we know all lines are from the same buffer */
    free(init_conf[0]);
    free(init_conf);

    while (1)
        sleep(1);

    return 0;
}
