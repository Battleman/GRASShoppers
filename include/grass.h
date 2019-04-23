/*
 * grass.h : GRASS miscellaneous header
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#ifndef GRASS_H
#define GRASS_H

#define DEBUG true

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



#define TOKENS_DELIM " \t\r\n" 

struct User {
    const char* uname;
    const char* pass;

    bool isLoggedIn;
};

struct Command {
    const char* cname;
    const char* cmd;
    const char* params;
};

void hijack_flow();

/*
 * Parses received line into a specified number of tokens.
 *
 * @parameter cmd
 *   Placeholder array pre-initialized with n_tok elements that will contain
 *   the parsed command and arguments. The last element will always be NULL.
 * @parameter line
 *   Line to parse, must be NULL-terminated.
 * @parameter n_tok
 *   Maximum number of tokens.
 */

void parse_line(char** cmd, char* line, int n_tok);

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

#endif /* GRASS_H */
