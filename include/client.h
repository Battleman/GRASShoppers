/*
 * client.h : Client header.
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <poll.h>

#include "grass.h"

enum ClientCommandID { GET, PUT, EXIT };

struct ClientCommand {
    const char *keyword;
    const enum ClientCommandID id;
};

#define SIZE_BUFFER 1024
#define FORMAT_SCANF "%1023[ -~]" /* from ' ' to '~' (all ASCII printables) */

#endif /* CLIENT_H */
