#include "grass.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>

/* ============================ GLOBAL VARIABLES ============================ */
#define SIZE_VARCONFS 3
static const struct ConfigVar conf_vars[SIZE_VARCONFS] = {
    {"base", BASE},
    {"port", PORT},
    {"user", USER}};
const char *ALL_COMMANDS[NUM_ALLOWED_COMMANDS] = {"ls", "login", "pass",
                                                  "ping", "cd", "mkdir",
                                                  "rm", "get", "put",
                                                  "grep", "date", "whoami",
                                                  "w", "logout", "exit"};
const char base[SIZE_BUFFER];
const int port;

const struct User users[SIZE_USERS];
int n_users = 0;

/* =============================== FUNCTIONS ================================ */

static void parse_conf_var(char *line, struct ConfigVar conf)
{
    char format[SIZE_BUFFER] = {0};
    strcat(format, conf.keyword);

    switch (conf.id)
    {
    case BASE:
        strcat(format, " %1023[A-Za-z0-9 ./\\-_]");
        sscanf(line, format, base);
        break;
    case PORT:
        strcat(format, " %d\n");
        sscanf(line, format, &port);
        break;
    case USER:
        if (n_users < SIZE_USERS)
        {
            strcat(format, " %1023[A-Za-z0-9_] %1023[ -~]\n");
            sscanf(line, format, users[n_users].name, users[n_users].pass);
            n_users++;
        }
        else
        {
            perror("Too many users.");
        }
        break;
    default:
        return;
    }
}

void hijack_flow(void)
{
    printf("Method hijack: Accepted\n");
}

void parse_conf_file(char const *filename)
{
    char buffer[SIZE_BUFFER] = {0};
    size_t i = 0;
    FILE *fp = NULL;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        perror("Failed opening config file.");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, SIZE_BUFFER, fp))
    {
        buffer[SIZE_BUFFER - 1] = '\0';
        /* If not a comment, checks if match with a configuration variable */
        if (buffer[0] != '#')
        {
            for (i = 0; i < SIZE_VARCONFS; ++i)
            {
                if (strstr(buffer, conf_vars[i].keyword))
                {
                    parse_conf_var(buffer, conf_vars[i]);
                }
            }
        }
    }

    fclose(fp);
}

int split_args(char **args, char *line, size_t n_tok)
{
    size_t idx = 0;
    char *token = NULL;

    token = strtok(line, TOKENS_DELIM);
    while (token != NULL && idx < (n_tok - 1))
    {
        args[idx++] = token;
        token = strtok(NULL, TOKENS_DELIM);
    }

    args[idx] = NULL;
    return idx;
}

int launch(char *output, int output_len, char **cmd)
{
    pid_t pid = 0;
    int status = 0;
    int comm[2];
    struct pollfd pfd;

    /* Creates a pipe between forked proceses */
    if (pipe(comm) == -1)
    {
        perror("Pipe failed");
        return -1; /* Error */
    }

    /* Forks the command execution */
    pid = fork();

    if (pid == 0)
    {
        /* Child process */

        /* Redirects stdout and stderr to communication link */
        dup2(comm[1], STDOUT_FILENO);
        dup2(comm[1], STDERR_FILENO);

        close(comm[0]);
        close(comm[1]);

        execvp(cmd[0], cmd);

        perror("Execution failed");
        exit(EXIT_FAILURE); /* Forces child to die */
    }
    else if (pid < 0)
    {
        /* Error forking */
        perror("Fork failed");
        return -1; /* Error */
    }
    else
    {
        /* Parent process */

        /* Waits for child to die (#novaxx) */
        waitpid(pid, &status, 0);

        /* Reads communication from child process if available */
        pfd.fd = comm[0];
        pfd.events = POLLRDNORM;
        if (poll(&pfd, 1, 0) > 0)
        {
            read(comm[0], output, output_len);
            output[output_len - 1] = '\0';
        }
        else
        {
            output[0] = '\0';
        }

        close(comm[0]);
        close(comm[1]);
    }

    return status;
}

int no_strange_char(char *check_str)
{
    const char *invalid_characters = "|;&$()/*#<>=!+%";
    char *c = check_str;
    while (*c)
    {
        if (strchr(invalid_characters, *c))
        {
            return 1;
        }
        c++;
    }
    return 0;
}
int check_args(char **args, char *output, size_t len_output, size_t num_args)
{
    size_t idx = 0;
    const char *command = args[0];
    int valid_token = 0;
    // check no invalid char
    for (idx = 0; idx < num_args; idx++)
    {
        valid_token = no_strange_char(args[idx]);
        if (valid_token != 0)
        {
            strncpy(output, "Invalid character in command", len_output);
            return valid_token;
        }
        idx++;
    }

    //check no invalid command
    for (idx = 0; idx < NUM_ALLOWED_COMMANDS; idx++)
    {
        if (strcmp(command, ALL_COMMANDS[idx]) == 0)
        {
            break;
        }
    }
    if (idx == NUM_ALLOWED_COMMANDS)
    {
        //not an allowed command
        strncpy(output, "Command not allowed", len_output);
        return 2;
    }

    //check number of parameters
    if ((strcmp(command, "ping") == 0 || strcmp(command, "ls") == 0 ||
         strcmp(command, "date") == 0 || strcmp(command, "whoami") == 0 ||
         strcmp(command, "w") == 0 || strcmp(command, "logout") == 0 ||
         strcmp(command, "exit") == 0) && num_args != 1)
    {
        strncpy(output, "This command can't have arguments", len_output);
        return 3;
    }
    if ((strcmp(command, "login")  == 0 || strcmp(command, "password") == 0 ||
         strcmp(command, "cd") == 0 || strcmp(command, "mkdir") == 0 ||
         strcmp(command, "rm") == 0 || strcmp(command, "grep") == 0 ||
         strcmp(command, "get")== 0 )&& num_args != 2)
    {
        strncpy(output, "This command needs exactly 1 arguments", len_output);
        return 3;
    }
    if (strcmp(command, "put") == 0  && num_args != 3)
    {
        strncpy(output, "This command needs exactly 2 arguments", len_output);
        return 3;
    }
    return 0;
}