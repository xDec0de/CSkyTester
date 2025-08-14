#ifndef SKY_ASSERTIONS_H
# define SKY_ASSERTIONS_H

/* stdout & fprintf */
#include <stdio.h>

/* exit */
#include <stdlib.h>

/* Global variables for configuration */

/**
 * @brief The tip to display if the next assertion fails. This can be
 * used to provide detailed assertion fail descriptions to help
 * the developer fix the issue.
 */
char *CST_FAIL_TIP = NULL;

/*
 - Internal assertion logic
 */

#define CST_ASSERT(expr, func, msg) do {\
	if ((expr)) {\
		CST_FAIL_TIP = NULL;\
		break ;\
	}\
	fprintf(stderr, msg, #func);\
	if (CST_FAIL_TIP != NULL)\
		fprintf(stderr, "\n   - %s", CST_FAIL_TIP);\
	CST_FAIL_TIP = NULL;\
	exit(EXIT_FAILURE);\
} while (0)

/*
 - Assertions - Bool
 */

/**
 * @brief Asserts that the provided `expr`ession is `true`.
 * If `expr` evaluates to `false`, the test will fail.
 * 
 * @param expr The expression to evaluate (Generally just a function call).
 */
#define ASSERT_TRUE(expr) CST_ASSERT(expr, expr, "Got FALSE when expecting TRUE from %s");

/**
 * @brief Asserts that the provided `expr`ession is `false`.
 * If `expr` evaluates to `true`, the test will fail.
 * 
 * @param expr The expression to evaluate (Generally just a function call).
 */
#define ASSERT_FALSE(expr) CST_ASSERT(!expr, expr, "Got TRUE when expecting FALSE from %s");

#endif
