/*
 * server.h : Server header.
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#ifndef SERVER_H
#define SERVER_H

#define PORT_FILE_LOADING 7777

#define MAX_CLIENTS 16

#define MSG_WELCOME "Welcome to GRASShoppersh!\n"

#define ERR_UNKNOWN "Execution failed: Unrecognized command\n"
#define ERR_INVALID "Execution failed: Invalid character detected\n"
#define ERR_PARAMS "Execution failed: Incorrect number of parameters\n"
#define ERR_FAILED "Execution failed: Wrong parameters\n"
#define ERR_UNAUTHORIZED "Execution failed: Unauthorized\n"

#define FILENAME_CONFIG "grass.conf"

#endif /* SERVER_H */
