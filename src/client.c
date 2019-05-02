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



#define PRINT_EXHAUSTIVE_READ(valread, buffer, size, pfd) do { \
    do { \
        valread = read(pfd.fd, buffer, size-1); \
        buffer[valread] = '\0'; \
        printf("%s", buffer); \
    } while (poll(&pfd, 1, 0) > 0); \
    printf("\n"); \
} while (0)




int main(void) {
    char buffer[SIZE_BUFFER] = {0};
    int sock = 0, valread = 0;
    struct pollfd pfd;

    /* Connects to server */
    if ((sock = connect_sock(IP_ADDR, PORT)) == -1) {
        exit(EXIT_FAILURE);
    }
    pfd.fd = sock;
    pfd.events = POLLRDNORM;

    /* Reads welcome message */
    PRINT_EXHAUSTIVE_READ(valread, buffer, SIZE_BUFFER, pfd);

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
