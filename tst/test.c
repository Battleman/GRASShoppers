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

#include "test_connect.h"
#include "test_grass.h"



#define TESTS_NUMBER 2

void (*tests[TESTS_NUMBER]) (void) = {
    connect_all_tests, grass_all_tests
};

unsigned int tests_run = 0;
unsigned int tests_ok = 0;

/*
 * Runs *_all_tests() functions.
 *
 * @return
 *   0 (all tests passed), 1 (at least one test failed).
 */

int main(void) {
    size_t i = 0;

    /* Runs the tests */
    for (i = 0; i < TESTS_NUMBER; ++i) {
        (*tests[i])();
    }

    /* Prints summary */
    printf("============================================================\n");
    printf("Tests run: [%u/%u]\n", tests_ok, tests_run);

    return (tests_ok != tests_run);
}
