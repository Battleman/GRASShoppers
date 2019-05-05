/*
 * grass.h : GRASS miscellaneous header
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#ifndef GRASS_H
#define GRASS_H

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>

#define SIZE_BUFFER 1024
#define SIZE_ARGS 16
#define SIZE_USERS 16

#define TOKENS_DELIM " \t\r\n"

enum ConfigID {
    BASE, PORT, USER
};

struct ConfigVar {
    const char *keyword;
    const enum ConfigID id;
};

struct User {
    const char name[SIZE_BUFFER];
    const char pass[SIZE_BUFFER];

    bool logged;
};

enum ServerCommandID {
    LS, LOGIN, PASS, PING, CD, MKDIR, RM, SERV_GET, SERV_PUT, GREP, DATE,
    WHOAMI, W, LOGOUT, SERV_EXIT,

    SERV_CMD_ERR_UNKNOWN=-1, SERV_CMD_ERR_PARAMS=-2, SERV_CMD_ERR_INVALID=-3,
    SERV_CMD_ERR_UNAUTHORIZED=-4
};

struct ServerCommand {
    const char *keyword;
    const enum ServerCommandID id;
    const unsigned char n_params;
    const bool privileged;
};

void hijack_flow(void);

/*
 * Parses given configuration file into global variables.
 *
 * @parameter filename
 *   Path to the configuration file (typically, "grass.conf").
 */

void parse_conf_file(char const *filename);

/*
 * Splits received line into a specified number of tokens.
 *
 * @parameter args
 *   Placeholder array pre-initialized with n_tok elements that will contain
 *   the parsed command and arguments. The last element will always be NULL.
 * @parameter line
 *   Line to parse, must be NULL-terminated.
 * @parameter n_tok
 *   Maximum number of tokens.
 */

int split_args(char **args, char *line, size_t n_tok);

/*
 * Executes indexed command and transfers output to socket.
 *
 * @parameter args
 *   Command and arguments to execute.
 * @parameter idx
 *   Index of the command to execute.
 * @parameter user
 *   Pointer to the struct of the current user (or NULL if not logged).
 * @parameter sock
 *   Socket to which the output needs to be sent.
 *
 * @return
 *   Number of bytes successfully sent.
 */

int execute(char **args, size_t idx, struct User **user, int sock);

/*
 * Checks correctness and authorization of command and arguments.
 *
 * @parameter args
 *   Tokenized array (with split_args) containing:
 *      args[0]:      Command to be executed.
 *      args[1+]:     Arguments to command.
 *      args[n_args]: NULL (literal).
 * @parameter user
 *    Pointer to the struct of the current user (or NULL if not logged).
 * @parameter n_args
 *    Number of tokens contained in args.
 *
 * @return
 *   Index of the command to be executed or a negative error value:
 *      -1:           Command is not recognized,
 *      -2:           Number of arguments does not match.
 *      -3:           Tokens contain invalid characters.
 */

int check_args(char **args, struct User* user, size_t num_args);

#endif /* GRASS_H */
