/*
 * server.c : Server main file.
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#include "connect.h"
#include "grass.h"
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct Command **cmdlist;
static int numCmds;

extern int port;


/* Helper function to run commands in unix. */
/*void run_command(const char* command, int sock){

}*/


/*
 * Send a file to the client as its own thread
 *
 * fp: file descriptor of file to send
 * sock: socket that has already been created.
 */
/*void send_file(int fp, int sock) {

}*/

/*
 * Send a file to the server as its own thread
 *
 * fp: file descriptor of file to save to.
 * sock: socket that has already been created.
 * size: the size (in bytes) of the file to recv
 */
/*void recv_file(int fp, int sock, int size) {

}*/

/* Server side REPL given a socket file descriptor */
/*void * connection_handler(void* sockfd) {

}*/

/*
 * search all files in the current directory
 * and its subdirectory for the pattern
 *
 * pattern: an extended regular expressions.
 * Output: A line seperated list of matching files' addresses
 */
/*void search(char *pattern) {

}*/

int main(void) {
    char buffer[SIZE_BUFFER] = {0};
    char **cmd;
    int sock = 0, valread = 0;

    if ((cmd = calloc(SIZE_ARGS, sizeof(char*))) == NULL) {
        perror("Arguments allocation failed");
        exit(EXIT_FAILURE);
    }

    /* Initializes server */
    parse_conf_file(FILENAME_CONFIG);

    /* Creates socket */
    sock = accept_sock(port);

    /* Sends welcome message */
    send(sock, MSG_WELCOME, MSG_WELCOME_LEN, 0);

    /* First read */
    valread = read(sock, buffer, SIZE_BUFFER);

    while (valread > 0) {
        printf("%s\n", buffer);

        /* Tokenizes the line into arguments */
        split_args(cmd, buffer, SIZE_ARGS);

        /* Sends command output if correctly executed */
        launch(buffer, SIZE_BUFFER, cmd);
        send(sock, buffer, SIZE_BUFFER, 0);

        /* Loops over */
        memset(buffer, 0, SIZE_BUFFER);
        valread = read(sock, buffer, SIZE_BUFFER);
    }

    /* Cleans up */
    free(cmd);
    close(sock);

    return 0;
}
