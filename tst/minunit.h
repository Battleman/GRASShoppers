/*
 * minunit.h : Minimal unit testing
 *
 * Source: http://www.jera.com/techinfo/jtns/jtn002.html
 */

#include <stdio.h>



extern unsigned int tests_run;
extern unsigned int tests_ok;

#define mu_assert(message, test) do { \
    if (!(test)) { \
        return message; \
    } \
} while (0)

#define mu_run_test(test, name) do { \
    char *message; \
    test_head(name); \
    message = test(); \
    tests_run++; \
    if (message) { \
        test_error(message); \
    } \
    else { \
        tests_ok++; \
        test_end; \
    } \
} while (0)

#define test_head(name) do { printf("Test %s:\t",name); } while (0)
#define test_end do { printf("[OK]\n"); } while (0)
#define test_error(message) do { printf("[FAILED]\n%s\n", message); } while(0)
