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
    {"user", USER}
};

const char base[SIZE_BUFFER];
const int port;

const struct User users[SIZE_USERS];
int n_users = 0;

/* =============================== FUNCTIONS ================================ */

static void parse_conf_var(char *line, struct ConfigVar conf) {
    char format[SIZE_BUFFER] = {0};
    strcat(format, conf.keyword);

    switch (conf.id) {
        case BASE:
            strcat(format, " %1023[A-Za-z0-9 ./\\-_]");
            sscanf(line, format, base);
            break;
        case PORT:
            strcat(format, " %d\n");
            sscanf(line, format, &port);
            break;
        case USER:
            if (n_users < SIZE_USERS) {
                strcat(format, " %1023[A-Za-z0-9_] %1023[ -~]\n");
                sscanf(line, format, users[n_users].name, users[n_users].pass);
                n_users++;
            } else {
                perror("Too many users.");
            }
            break;
        default:
            return;
    }
}

void hijack_flow(void) {
    printf("Method hijack: Accepted\n");
}

void parse_conf_file(char const *filename) {
    char buffer[SIZE_BUFFER] = {0};
    size_t i = 0;
    FILE *fp = NULL;

    if ((fp = fopen(filename, "r")) == NULL) {
        perror("Failed opening config file.");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, SIZE_BUFFER, fp)) {
        buffer[SIZE_BUFFER-1] = '\0';
        /* If not a comment, checks if match with a configuration variable */
        if (buffer[0] != '#') {
            for (i = 0; i < SIZE_VARCONFS; ++i) {
                if (strstr(buffer, conf_vars[i].keyword)) {
                    parse_conf_var(buffer, conf_vars[i]);
                }
            }
        }
    }

    fclose(fp);
}

void split_args(char **args, char *line, size_t n_tok) {
    size_t idx = 0;
    char *token = NULL;

    token = strtok(line, TOKENS_DELIM);
    while (token != NULL && idx < (n_tok - 1)) {
        args[idx++] = token;
        token = strtok(NULL, TOKENS_DELIM);
    }

    args[idx] = NULL;
}

int launch(char *output, int output_len, char **cmd) {
    pid_t pid = 0;
    int status = 0;
    int comm[2];
    struct pollfd pfd;

    /* Creates a pipe between forked proceses */
    if (pipe(comm) == -1) {
        perror("Pipe failed");
        return -1; /* Error */
    }

    /* Forks the command execution */
    pid = fork();

    if (pid == 0) {
        /* Child process */

        /* Redirects stdout and stderr to communication link */
        dup2(comm[1], STDOUT_FILENO);
        dup2(comm[1], STDERR_FILENO);

        close(comm[0]);
        close(comm[1]);

        execvp(cmd[0], cmd);

        perror("Execution failed");
        exit(EXIT_FAILURE); /* Forces child to die */
    } else if (pid < 0) {
        /* Error forking */
        perror("Fork failed");
        return -1; /* Error */
    } else {
        /* Parent process */

        /* Waits for child to die (#novaxx) */
        waitpid(pid, &status, 0);

        /* Reads communication from child process if available */
        pfd.fd = comm[0];
        pfd.events = POLLRDNORM;
        if (poll(&pfd, 1, 0) > 0) {
            read(comm[0], output, output_len);
            output[output_len - 1] = '\0';
        } else {
            output[0] = '\0';
        }

        close(comm[0]);
        close(comm[1]);
    }

    return status;
}


int check_args(char **args, char *line, size_t n_tok){
    printf(args[0]);
    if(strcmp(args[0], "exit") == 0){
        line = "Byby";
        printf("By friend");
        return 0;
    } else{
        if(strcmp(args[0], "cd")){
            line = "Ain't gonna cd bro";
            return 0;
        } else{
            return 0;
        }
    }
    return 1;
}