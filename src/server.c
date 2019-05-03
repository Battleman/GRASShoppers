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

<<<<<<< HEAD
/* Helper function to run commands in unix. */
/*void run_command(const char* command, int sock){
=======


static void *server_get(void* args) {
    struct FileLoading* fload = (struct FileLoading*) args;
>>>>>>> 12aaaea9d91b0b4d4cf2f6e1bf6be1d121498bdb

    fload->sock = accept_sock(port);

<<<<<<< HEAD
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
=======
    send_file(fload);
    close(fload->sock);

    free(fload);
    return NULL;
}
>>>>>>> 12aaaea9d91b0b4d4cf2f6e1bf6be1d121498bdb

static void *server_put(void* args) {
    struct FileLoading* fload = (struct FileLoading*) args;

    fload->sock = accept_sock(port);

    recv_file(fload);
    close(fload->sock);

    free(fload);
    return NULL;
}

<<<<<<< HEAD
int main(void)
{
=======
static void *main_loop(void* args) {
    int sock = *((int *)args), valread = 0;
>>>>>>> 12aaaea9d91b0b4d4cf2f6e1bf6be1d121498bdb
    char buffer[SIZE_BUFFER] = {0};
    char **cmd;

<<<<<<< HEAD
    if ((cmd = calloc(SIZE_ARGS, sizeof(char *))) == NULL){
        perror("Arguments allocation failed");
        exit(EXIT_FAILURE);
=======
    if ((cmd = calloc(SIZE_ARGS, sizeof(char*))) == NULL) {
        close(sock);
        return NULL;
>>>>>>> 12aaaea9d91b0b4d4cf2f6e1bf6be1d121498bdb
    }

    /* Sends welcome message */
    send(sock, MSG_WELCOME, MSG_WELCOME_LEN, 0);

    /* First read */
    valread = read(sock, buffer, SIZE_BUFFER);

    while (valread > 0) {
        printf("%s\n", buffer);

        /* Tokenizes the line into arguments */
        split_args(cmd, buffer, SIZE_ARGS);
<<<<<<< HEAD
        if (!check_args(cmd, buffer, SIZE_ARGS)){
            printf("Nope, not valid");
            send(sock, MSG_ERROR, MSG_ERROR_LEN, 0);
            valread = 0;
        } else {
            /* Sends command output if correctly executed */
            if (launch(buffer, SIZE_BUFFER, cmd) == 0)
            {
                send(sock, buffer, SIZE_BUFFER, 0);
            }
            else
            {
                send(sock, MSG_ERROR, MSG_ERROR_LEN, 0);
            }

            /* Loops over */
            memset(buffer, 0, SIZE_BUFFER);
            valread = read(sock, buffer, SIZE_BUFFER);
        }
=======

        /* Sends command output */
        launch(buffer, SIZE_BUFFER, cmd);
        send(sock, buffer, SIZE_BUFFER, 0);

        /* Loops over */
        memset(buffer, 0, SIZE_BUFFER);
        valread = read(sock, buffer, SIZE_BUFFER);
>>>>>>> 12aaaea9d91b0b4d4cf2f6e1bf6be1d121498bdb
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
