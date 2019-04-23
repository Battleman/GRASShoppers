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

int connect_sock(char * ip_addr, int port);

#endif /* CONNECT_H */
