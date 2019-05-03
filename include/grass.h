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



#define DEBUG true

#define SIZE_BUFFER 1024
#define SIZE_ARGS 16
#define SIZE_USERS 16

#define TOKENS_DELIM " \t\r\n"

enum ConfigID { BASE, PORT, USER };

struct ConfigVar {
    const char *keyword;
    const enum ConfigID id;
};

struct User {
    const char name[SIZE_BUFFER];
    const char pass[SIZE_BUFFER];

    bool isLoggedIn;
};

struct Command {
    const char* cname;
    const char* cmd;
    const char* params;
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

void split_args(char **args, char *line, size_t n_tok);

/*
 * Executes parsed line and transfers its output and return status.
 *
 * @parameter output
 *   Placeholder array pre-initialized with output_len elements that will
 *   contain the output of executed command.
 * @parameter output_len
 *   Available length of output string.
 * @parameter cmd
 *   Command and arguments to execute.
 *
 * @return
 *   Return status of the command executed.
 */

int launch(char *output, int output_len, char **cmd);

/*

*/
int check_args(char **args, char *line, size_t n_tok);

#endif /* GRASS_H */
