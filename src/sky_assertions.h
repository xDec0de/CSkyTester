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
char *__CST_FAIL_TIP__ = NULL;

/*
 - Fail trigger macro
 */

#define CST_FAIL(expr, msg) do {\
	fprintf(stderr, msg, #expr);\
	if (__CST_FAIL_TIP__ != NULL)\
		fprintf(stderr, "\n   - %s", __CST_FAIL_TIP__);\
	exit(EXIT_FAILURE);\
} while (0)

/*
 - Assertions - Bool
 */

#define ASSERT_TRUE(expr) do {\
	if (!(expr))\
		CST_FAIL(#expr, "Got FALSE when expecting TRUE from %s");\
	__CST_FAIL_TIP__ = NULL;\
} while (0)

#define ASSERT_FALSE(expr) do {\
	if ((expr))\
		CST_FAIL(#expr, "Got TRUE when expecting FALSE from %s");\
	__CST_FAIL_TIP__ = NULL;\
} while (0)

#endif
