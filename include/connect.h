/*
 * connect.h : Connections handler.
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#ifndef CONNECT_H
#define CONNECT_H

#include "grass.h"

struct FileLoading {
    int sock;
    char ipv4_addr[16];
    int port;
    char filename[SIZE_BUFFER];
    unsigned long size;
};

/*
 * Accepts new connections and transfers its socket.
 *
 * @parameter port
 *   Port to listen to new connections.
 *
 * @return
 *   File descriptor of accepted socket.
 */

int accept_sock(int port);

/*
 * Connects to an IP address and port.
 *
 * @parameter ip_addr
 *   IPv4 or IPv6 address to connect to, must be NULL-terminated.
 * @parameter port
 *   Port to connect to.
 *
 * @return
 *   File descriptor of accepted socket.
 */

int connect_sock(char const *ip_addr, int port);

/*
 * Retrieves IP address from socket.
 *
 * @parameter ip_addr
 *   Placeholder array pre-initialized with n_ip elements that will contain the
 *   IP address of the socket file descriptor. The last element will always
 *   be NULL.
 * @parameter n_ip
 *   Available length of IP address string.
 * @parameter sock
 *   Socket for which IP address has to be recovered.
 *
 */

void get_ip(char *ip_addr, int n_ip, int sock);

/*
 * Receives content from socket and writes it in a file.
 *
 * @parameter fload
 *   Structure containing all parameters for file receiving. The structure is
 *   supposed correct.
 *
 * @return
 *   1 (True) if file could have been received errorless, 0 (False) otherwise.
 */

int recv_file(struct FileLoading *fload);

/*
 * Reads a file and sends its content to a socket.
 *
 * @parameter fload
 *   Structure containing all parameters for file sending. The structure is
 *   supposed correct.
 *
 * @return
 *   1 (True) if file could have been sent errorless, 0 (False) otherwise.
 */

int send_file(struct FileLoading *fload);

#endif /* CONNECT_H */
