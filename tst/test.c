/*
 * tests.c : Fancy minimalist framework for testing structures.
 *
 * Source: http://www.jera.com/techinfo/jtns/jtn002.html
 *
 *  Created on: Feb 26, 2019
 *      Author: Aymeric Genêt
 */

#include <stdio.h>
#include "minunit.h"

/* assert TESTS_NUMBER < MAX_INT */
#define TESTS_NUMBER 0

int (*tests[TESTS_NUMBER])() = {};

/*
 *  Runs all_tests() functions.
 *
 * @Return: 0 (all tests passed), 1 (one test failed)
 */

int main(int argc, char *argv[]) {
    size_t i;
    unsigned int result;

    /* runs the tests */
    result = 0;
    for (i = 0; i < TESTS_NUMBER; ++i) {
        result += ((*tests[i])() != 0);
    }

    /* prints summary */
    printf("============================================================\n");
    printf("Tests run: [%u/%i]\n", TESTS_NUMBER-result, TESTS_NUMBER);

    return (result != 0);
}
