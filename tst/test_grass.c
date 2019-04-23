/*
 * test_grass.c
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#include "minunit.h"
#include "grass.h"

#include <stdio.h>



static char* test_parse_line(void) {

    return NULL;
}

void grass_all_tests(void) {
    printf("\n=======================  GRASS tests  ======================\n\n");
    mu_run_test(test_parse_line, "parse_line()");
    printf("\n");
}
