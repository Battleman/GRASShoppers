#define _GNU_SOURCE 1
#include "grass.h"
#include "server.h"
#include "connect.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>

/* ============================ GLOBAL VARIABLES ============================ */
#define SIZE_CONF_VARS 3
static const struct ConfigVar CONF_VARS[SIZE_CONF_VARS] = {
    {"base", BASE},
    {"port", PORT},
    {"user", USER}
};

#define SIZE_SERVER_COMMANDS 15
static const struct ServerCommand SERV_CMD[SIZE_SERVER_COMMANDS] = {
    {"ls", LS, 1, true},
    {"login", LOGIN, 2, false},
    {"pass", PASS, 2, false},
    {"ping", PING, 2, false},
    {"cd", CD, 2, true},
    {"mkdir", MKDIR, 2, true},
    {"rm", RM, 2, true},
    {"get", SERV_GET, 2, true},
    {"put", SERV_PUT, 3, true},
    {"grep", GREP, 2, true},
    {"date", DATE, 1, true},
    {"whoami", WHOAMI, 1, true},
    {"w", W, 1, true},
    {"logout", LOGOUT, 1, true},
    {"exit", SERV_EXIT, 1, false}
};

const char *CHARACTERS_INVALID = "|;&$()/*#<>=!+%";

const char base[SIZE_BUFFER];
const int port;

struct User *users;
size_t n_users = 0;
size_t max_users = SIZE_USERS;

pthread_t *serv_get_thread = NULL;
pthread_t *serv_put_thread = NULL;

/* =============================== FUNCTIONS ================================ */
/* Static */

static void parse_conf_var(char *line, struct ConfigVar conf)
{
    char format[SIZE_BUFFER] = {0};
    strcat(format, conf.keyword);

    switch (conf.id)
    {
    case BASE:
        strcat(format, " %1023[A-Za-z0-9 ./\\-_]");
        sscanf(line, format, base);
        if (chdir(base) != 0) {
            perror("Failed changing directory to base folder.");
        }
        break;
    case PORT:
        strcat(format, " %d\n");
        sscanf(line, format, &port);
        break;
    case USER:
        if (n_users < max_users) {
            strcat(format, " %1023[A-Za-z0-9_] %1023[ -~]\n");
            sscanf(line, format, users[n_users].name, users[n_users].pass);
            n_users++;
        } else {
            users = mremap(users, max_users*sizeof(struct User), 2*max_users*sizeof(struct User),
                           MREMAP_MAYMOVE);
            max_users *= 2;
        }
        break;
    default:
        return;
    }
}

static int launch(char **cmd, int sock) {
    char buffer[SIZE_BUFFER] = {0};
    pid_t pid = 0;
    int valread = 1;
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
        waitpid(pid, NULL, 0);

        /* Reads communication from child process if available */
        pfd.fd = comm[0];
        pfd.events = POLLRDNORM;
        buffer[SIZE_BUFFER - 1] = '\0';
        while (valread > 0 && poll(&pfd, 1, 0) > 0)
        {
            valread = read(comm[0], buffer, SIZE_BUFFER-1);
            valread = send(sock, buffer, valread, 0);
        }

        close(comm[0]);
        close(comm[1]);
    }

    return valread;
}

static int no_strange_char(char const *check_str) {
    char const *c = check_str;

    while (*c) {
        if (strchr(CHARACTERS_INVALID, *c)) {
            return SERV_CMD_ERR_INVALID;
        }
        c++;
    }

    return 0;
}

static void *server_get(void* args) {
    struct FileLoading* fload = (struct FileLoading*) args;

    fload->sock = accept_sock(port);

    send_file(fload);
    close(fload->sock);

    free(fload);
    return NULL;
}

static void *server_put(void* args) {
    struct FileLoading* fload = (struct FileLoading*) args;

    fload->sock = accept_sock(port);

    recv_file(fload);
    close(fload->sock);

    free(fload);
    return NULL;
}

/* =============================== FUNCTIONS ================================ */
/* Global */

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

    users = mmap(NULL, MAX_CLIENTS*sizeof(struct User), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    while (fgets(buffer, SIZE_BUFFER, fp))
    {
        buffer[SIZE_BUFFER - 1] = '\0';
        /* If not a comment, checks if match with a configuration variable */
        if (buffer[0] != '#')
        {
            for (i = 0; i < SIZE_CONF_VARS; ++i)
            {
                if (strstr(buffer, CONF_VARS[i].keyword))
                {
                    parse_conf_var(buffer, CONF_VARS[i]);
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

int check_args(char **args, struct User* user, size_t n_args) {
    size_t idx = 0;
    const char *command = args[0];
    int valid_token = 0;

    /* Checks for invalid char */
    for (idx = 0; idx < n_args; idx++) {
        valid_token = no_strange_char(args[idx]);

        if (valid_token != 0) {
            return valid_token;
        }
        idx++;
    }

    /* Checks for invalid command */
    for (idx = 0; idx < SIZE_SERVER_COMMANDS; idx++) {
        if (strcmp(command, SERV_CMD[idx].keyword) == 0) {
            if (n_args == SERV_CMD[idx].n_params) {
                if (SERV_CMD[idx].privileged && (user == NULL || !user->logged)) {
                    return SERV_CMD_ERR_UNAUTHORIZED; /* Unauthorized command */
                }
                return idx; /* Correct command and aguments */
            } else {
                return SERV_CMD_ERR_PARAMS; /* Invalid number of arguments */
            }
        }
    }

    return SERV_CMD_ERR_UNKNOWN; /* Invalid command */
}

int execute(char **args, size_t idx, struct User **user, int sock) {
    char buffer[SIZE_BUFFER] = {0};
    struct FileLoading *fload = NULL;
    struct stat st;
    size_t i = 0;
    int valread = 0;

    switch (idx) {
    case GREP:
        /* Hacks command to match output */
        args[2] = "-d";
        args[3] = "skip";
        args[4] = "-l";
        args[5] = "*";
        args[6] = NULL;
        return launch(args, sock);

    case LS:
        /* Hacks command to match output */
        args[1] = "-l";
        args[2] = NULL;
        return launch(args, sock);

    case PING:
        /* Hacks command to match output */
        args[2] = "-c1";
        args[3] = NULL;
        return launch(args, sock);

    case LOGIN:
        /* Fails if already logged in */
        if ((*user) != NULL && (*user)->logged) {
            return send(sock, ERR_FAILED, strlen(ERR_FAILED), 0);
        }

        /* Checks if user exists and prepares struct */
        for (i = 0; i < n_users; ++i) {
            if (strcmp(users[i].name, args[1]) == 0 && !(users[i].logged)) {
                (*user) = &(users[i]);
                return send(sock, "\0", 1, 0);
            }
        }

        return send(sock, ERR_FAILED, strlen(ERR_FAILED), 0);

    case PASS:
        /* Fails if already logged in */
        if ((*user) != NULL && (*user)->logged) {
            return send(sock, ERR_FAILED, strlen(ERR_FAILED), 0);
        }

        /* Checks password */
        if (strcmp((*user)->pass, args[1]) == 0) {
            (*user)->logged = true;
            return send(sock, "\0", 1, 0);
        }
        
        return send(sock, ERR_FAILED, strlen(ERR_FAILED), 0);

    case W:
        /* Checks all users if logged */
        for (i = 0, valread = 0; i < n_users; ++i) {
            if (users[i].logged) {
                snprintf(buffer, "%s\n", users[i].name);
                buffer[SIZE_BUFFER-1] = '\0';
                valread += send(sock, buffer, strlen(buffer), 0);
            }
        }

        if (valread == 0) {
            return send(sock, "\0", 1, 0);
        }

        return valread;

    case LOGOUT:
        (*user)->logged = false;

        return send(sock, "\0", 1, 0);

    case SERV_GET:
        /* Blocks if serv_get_thread exists. */
        if (serv_get_thread == NULL) {
            serv_get_thread = malloc(sizeof(pthread_t));
        } else {
            pthread_join(*serv_get_thread, NULL);
        }

        /* Checks that file exists and recovers its size */
        if (stat(args[1], &st) != 0) {
            free(serv_get_thread);
            serv_get_thread = NULL;
            return send(sock, ERR_FAILED, strlen(ERR_FAILED), 0);
        }

        /* Prepares thread */
        fload = malloc(sizeof(struct FileLoading));
        strncpy(fload->filename, args[1], SIZE_BUFFER-1);
        fload->filename[SIZE_BUFFER-1] = '\0';
        fload->port = PORT_FILE_LOADING;
        fload->size = st.st_size;

        pthread_create(serv_get_thread, NULL, server_get, fload);

        /* Prepares response */
        snprintf(buffer, SIZE_BUFFER-1, "get port: %d size: %lu", fload->port, fload->size);
        buffer[SIZE_BUFFER-1] = '\0';

        return send(sock, buffer, strlen(buffer), 0);

    case SERV_PUT:
        /* Blocks if serv_put_thread exists. */
        if (serv_put_thread == NULL) {
            serv_put_thread = malloc(sizeof(pthread_t));
        } else {
            pthread_join(*serv_put_thread, NULL);
        }

        /* Prepares thread */
        fload = malloc(sizeof(struct FileLoading));
        strncpy(fload->filename, args[1], SIZE_BUFFER-1);
        fload->filename[SIZE_BUFFER-1] = '\0';
        fload->port = PORT_FILE_LOADING;
        fload->size = strtoul(args[2], NULL, 10);

        pthread_create(serv_put_thread, NULL, server_put, fload);

        /* Prepares response */
        snprintf(buffer, SIZE_BUFFER-1, "put port: %d", fload->port);
        buffer[SIZE_BUFFER-1] = '\0';

        return send(sock, buffer, strlen(buffer), 0);

    case CD:
        if (chdir(args[1]) != 0) {
            return send(sock, ERR_FAILED, strlen(ERR_FAILED), 0);
        }
        return send(sock, "\0", 1, 0);

    case MKDIR:
    case RM:
    case DATE:
        return launch(args, sock);

    case WHOAMI:
        return send(sock, (*user)->name, strlen((*user)->name), 0);

    case SERV_EXIT:
        return -1;

    case SERV_CMD_ERR_UNAUTHORIZED:
        return send(sock, ERR_UNAUTHORIZED, strlen(ERR_UNAUTHORIZED), 0);
    case SERV_CMD_ERR_PARAMS:
        return send(sock, ERR_PARAMS, strlen(ERR_PARAMS), 0);
    case SERV_CMD_ERR_INVALID:
        return send(sock, ERR_INVALID, strlen(ERR_INVALID), 0);
    case SERV_CMD_ERR_UNKNOWN:
    default:
        return send(sock, ERR_UNKNOWN, strlen(ERR_UNKNOWN), 0);
    }
}
