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
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

extern int port;
extern struct User **users;



static int shell_loop(int sock) {
    int valread = 0, n_args = 0;
    int idx = -1;
    char buffer[SIZE_BUFFER] = {0};
    char **cmd;
    struct User *user = NULL;

    if ((cmd = calloc(SIZE_ARGS, sizeof(char*))) == NULL) {
        close(sock);
        return 1;
    }

    /* Sends welcome message */
    send(sock, MSG_WELCOME, strlen(MSG_WELCOME), 0);

    /* First read */
    valread = read(sock, buffer, SIZE_BUFFER);

    while (valread > 0) {
        printf("%s\n", buffer);

        /* Tokenizes the line into arguments */
        n_args = split_args(cmd, buffer, SIZE_ARGS);

        /* Parses command line */
        idx = check_args(cmd, user,n_args);

        /* Executes parsed command */
        valread = execute(cmd, idx, &user, sock);

        /* Loops over */
        memset(buffer, 0, SIZE_BUFFER);
        valread = read(sock, buffer, SIZE_BUFFER);
    }

    /* Cleans up */
    if (user != NULL) {
        user->logged = false;
    }
    free(cmd);
    close(sock);

    return 0;
}

int main(void) {
    int sock = 0;

    users = mmap(NULL, sizeof(struct User*), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    /* Initializes server */
    parse_conf_file(FILENAME_CONFIG);

    /* Creates socket and launches independent processes */
    do {
        sock = accept_sock(port);

        if (fork() == 0) {
            printf("Client accepted.\n");
            return shell_loop(sock);
        }
    } while (1);

    free(*users);
    munmap(users, sizeof(struct User*));

    return 0;
}

