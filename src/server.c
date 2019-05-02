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

extern int port;



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
    get_ip(buffer, SIZE_BUFFER, sock);
    printf("Client accepted.\nIP: %s\n", buffer);

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
