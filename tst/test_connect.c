/*
 * test_connect.c
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#include "minunit.h"
#include "connect.h"

#include <stdio.h>



static char* test_connect_sock(void) {

    return NULL;
}

static char* test_accept_sock(void) {

    return NULL;
}

void connect_all_tests(void) {
    printf("\n======================= CONNECT tests ======================\n\n");
    mu_run_test(test_connect_sock, "connect_sock()");
    mu_run_test(test_accept_sock, "accept_sock()");
    printf("\n");
}
