/*
 * test_grass.c
 *
 *  Created on: Apr 22, 2019
 *      Author: Aymeric Genêt
 */

#include "minunit.h"
#include "grass.h"

#include <stdio.h>


#define TEST_SIZE_ARGS 8
#define TEST_SIZE_LINE 64


static char* test_split_args(void) {
    char **args;
    char line[TEST_SIZE_LINE] = {0};

    args = calloc(TEST_SIZE_ARGS, sizeof(char*));

    mu_assert("Allocation failed.", args != NULL);

    /* ============================= Basic test ============================= */
    strcpy(line, "00000 1111\t222\r33\n4");
    split_args(args, line, TEST_SIZE_ARGS);

    mu_assert("Basic test failed (at 0).", strcmp(args[0], "00000") == 0);
    mu_assert("Basic test failed (at 1).", strcmp(args[1], "1111") == 0);
    mu_assert("Basic test failed (at 2).", strcmp(args[2], "222") == 0);
    mu_assert("Basic test failed (at 3).", strcmp(args[3], "33") == 0);
    mu_assert("Basic test failed (at 4).", strcmp(args[4], "4") == 0);
    mu_assert("Basic test failed (at 5).", args[5] == NULL);

    /* =========================== Outliers tests =========================== */
    strcpy(line, "");
    split_args(args, line, TEST_SIZE_ARGS);
    mu_assert("Empty test failed.", args[0] == NULL);

    strcpy(line, "     ");
    split_args(args, line, TEST_SIZE_ARGS);
    mu_assert("Whitespace-only test failed.", args[0] == NULL);

    strcpy(line, "   0 1 ");
    split_args(args, line, TEST_SIZE_ARGS);
    mu_assert("Whitespace-funny test failed (at 0).", strcmp(args[0], "0") == 0);
    mu_assert("Whitespace-funny test failed (at 1).", strcmp(args[1], "1") == 0);
    mu_assert("Whitespace-funny test failed (at 2).", args[2] == NULL);

    strcpy(line, "0 1 2 3 4 5 6 7");
    split_args(args, line, TEST_SIZE_ARGS);
    mu_assert("Size-overflowed test failed (at 0).", strcmp(args[0], "0") == 0);
    mu_assert("Size-overflowed test failed (at 1).", strcmp(args[1], "1") == 0);
    mu_assert("Size-overflowed test failed (at 2).", strcmp(args[2], "2") == 0);
    mu_assert("Size-overflowed test failed (at 3).", strcmp(args[3], "3") == 0);
    mu_assert("Size-overflowed test failed (at 4).", strcmp(args[4], "4") == 0);
    mu_assert("Size-overflowed test failed (at 5).", strcmp(args[5], "5") == 0);
    mu_assert("Size-overflowed test failed (at 6).", strcmp(args[6], "6") == 0);
    mu_assert("Size-overflowed test failed (at 7).", args[7] == NULL);

    strcpy(line, "+\"\'*ç%&/()=?!^[]${}-_.:,;<>\\|@#¼½¬|¢]´~");
    split_args(args, line, TEST_SIZE_ARGS);
    mu_assert("Special characters test failed (at 0).", strcmp(args[0], line) == 0);
    mu_assert("Special characters test failed (at 1).", args[1] == NULL);

    return NULL; /* Success */
}

void grass_all_tests(void) {
    printf("\n=======================  GRASS tests  ======================\n\n");
    mu_run_test(test_split_args, "split_args()");
    printf("\n");
}
