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

static void *main_loop(void* args) {
    int sock = *((int *)args), valread = 0;
    char buffer[SIZE_BUFFER] = {0};
    char **cmd;

    if ((cmd = calloc(SIZE_ARGS, sizeof(char*))) == NULL) {
        close(sock);
        return NULL;
    }

    /* Sends welcome message */
    send(sock, MSG_WELCOME, MSG_WELCOME_LEN, 0);

    /* First read */
    valread = read(sock, buffer, SIZE_BUFFER);

    while (valread > 0) {
        printf("%s\n", buffer);

        /* Tokenizes the line into arguments */
        split_args(cmd, buffer, SIZE_ARGS);

        /* Sends command output */
        launch(buffer, SIZE_BUFFER, cmd);
        send(sock, buffer, SIZE_BUFFER, 0);

        /* Loops over */
        memset(buffer, 0, SIZE_BUFFER);
        valread = read(sock, buffer, SIZE_BUFFER);
    }

    /* Cleans up */
    free(cmd);
    close(sock);

    return NULL;
}

int main(void) {
    pthread_t clients[MAX_CLIENTS];
    int *socks;
    size_t i = 0;

    if ((socks = calloc(MAX_CLIENTS, sizeof(int))) == NULL) {
        exit(EXIT_FAILURE);
    }

    /* Initializes server */
    parse_conf_file(FILENAME_CONFIG);

    /* Creates socket and launches independent thread */
    for (i = 0; i < MAX_CLIENTS; ++i) {
        socks[i] = accept_sock(port);
        printf("Client accepted.\n");
        pthread_create(&(clients[i]), NULL, main_loop, &(socks[i]));
    }

    /* Waits on each of them */
    for (i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(clients[i], NULL);
    }

    /* Cleans up */
    free(socks);

    return 0;
}
