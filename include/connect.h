/*
 * connect.h : Connections handler.
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#ifndef CONNECT_H
#define CONNECT_H


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

#endif /* CONNECT_H */
