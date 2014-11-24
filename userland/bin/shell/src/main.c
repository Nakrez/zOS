#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

# define COMMAND_INIT_SIZE 2

struct command {
    char **argv;

    size_t size;
    size_t max_size;
};

static struct command *command_new(void)
{
    struct command *command = malloc(sizeof (struct command));

    if (!command)
        return NULL;

    if (!(command->argv = malloc(sizeof (char *) * COMMAND_INIT_SIZE)))
    {
        free(command);

        return NULL;
    }

    command->max_size = COMMAND_INIT_SIZE;
    command->size = 0;

    return command;
}

static int command_append(struct command **command, char *arg)
{
    if ((*command)->max_size == (*command)->size)
    {
        struct command *tmp;

        (*command)->max_size *= 2;
        tmp = realloc(*command, sizeof (char *) * (*command)->max_size);

        if (!tmp)
        {
            (*command)->max_size /= 2;

            return -1;
        }

        *command = tmp;
    }

    (*command)->argv[(*command)->size++] = arg;

    return 0;
}

static void command_free(struct command *command)
{
    if (!command)
        return;

    if (command->argv)
        free(command->argv);

    free(command);
}

static void command_execute(struct command *command)
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "fork() failed\n");

        return;
    }

    if (pid)
    {
        int status;

        if ((pid = waitpid(pid, &status, 0)) < 0)
        {
            fprintf(stderr, "waitpid() failed (err = %i)\n", pid);

            return;
        }

        if (status != 0)
            fprintf(stderr, "shell: process returned %i\n", status);
    }
    else
    {
        char *bin;

        if (*command->argv[0] != '/')
        {
            /* 6 = /bin/ + '\0' */
            if (!(bin = malloc(strlen(command->argv[0] + 6))))
                exit(1);

            strcpy(bin, "/bin/");
            strcat(bin, command->argv[0]);
        }
        else
            bin = command->argv[0];

        execv(bin, command->argv);

        fprintf(stderr, "Cannot execute: %s\n", bin);

        exit(1);
    }
}

static void execute(char *buf)
{
    struct command *cmd = command_new();
    char *arg;
    char *rest;

    if (!cmd)
    {
        fprintf(stderr, "Memory exhausted\n");

        return;
    }

    while (*buf && *buf == ' ')
        buf++;

    if (!*buf)
    {
        command_free(cmd);

        return;
    }

    arg = strtok_r(buf, " ", &rest);

    while (arg)
    {
        if (command_append(&cmd, arg) < 0)
        {
            command_free(cmd);

            fprintf(stderr, "Memory exhausted\n");

            return;
        }

        while (*rest && *rest == ' ')
            rest++;

        arg = strtok_r(NULL, " ", &rest);
    }

    command_append(&cmd, NULL);

    command_execute(cmd);

    command_free(cmd);
}

int main(void)
{
    int ret;
    char buf[256];

    /* TODO: Let init handle it */
    while (1)
    {
        printf("root@zOS $ ");

        fflush(stdout);

        ret = read(STDIN_FILENO, buf, 256);

        if (ret < 0)
        {
            printf("\nread failed\n");

            continue;
        }

        if (ret > 1)
        {
            buf[ret - 1] = 0;

            execute(buf);
        }
    }

    return 0;
}
