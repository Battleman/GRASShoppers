/*
 * client.c : Client main file.
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#include "grass.h"
#include "connect.h"
#include "client.h"

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



/*
 * Send a file to the server as its own thread
 *
 * fp: file descriptor of file to send
 * d_port: destination port
 */
/*void send_file(int fp, int d_port) {
}*/

/*
 * Recv a file from the server as its own thread
 *
 * fp: file descriptor of file to save to.
 * d_port: destination port
 * size: the size (in bytes) of the file to recv
 */
/*void recv_file(int fp, int d_port, int size) {
}*/


/*
 * search all files in the current directory
 * and its subdirectory for the pattern
 *
 * pattern: an extended regular expressions.
 */
/*void search(char *pattern) {
}*/

int main(void) {
    char buffer[SIZE_BUFFER] = {0};
    int sock = 0, valread = 0;

    /* Connects to server */
    if ((sock = connect_sock(IP_ADDR, PORT)) == -1) {
        exit(EXIT_FAILURE);
    }

    /* Reads welcome message */
    valread = read(sock, buffer, SIZE_BUFFER);

    while (valread > 0) {
        /* Main loop */
        printf("%s\n", buffer);
        printf("> ");

        memset(buffer, 0, SIZE_BUFFER);
        scanf(FORMAT_SCANF, buffer); getchar();

        valread = send(sock, buffer, SIZE_BUFFER, 0);
        valread = read(sock, buffer, SIZE_BUFFER);
    }

    /* Cleans up */
    close(sock);

    return 0;
}
