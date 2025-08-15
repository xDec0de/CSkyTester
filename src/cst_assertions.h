#ifndef CST_ASSERTIONS_H
# define CST_ASSERTIONS_H

#include "cst_color.h"

/* stdout & fprintf */
#include <stdio.h>

/* exit */
#include <stdlib.h>

/* bool type */
#include <stdbool.h>

/* Custom test name macros with fallback */

#ifndef CST_FILE
# define CST_FILE "???"
#endif

#ifndef CST_TEST_NAME
# define CST_TEST_NAME CST_FILE
#endif

/* Global variables for configuration */

/**
 * @brief Whether to display the default assertion failure details.
 * If `true`, whenever an assertion fails, a default description
 * such as "Got TRUE when expecting FALSE from (function)" is displayed.
 * 
 * Default: `true`
 */
bool	CST_SHOW_FAIL_DETAILS = true;

/**
 * @brief The tip to display if the next assertion fails. This can be
 * used to provide detailed assertion fail descriptions to help
 * the developer fix the issue.
 * 
 * This will reset back to `NULL` once any assertion occurs, no
 * matter if it fails or passes.
 * 
 * Default: `NULL`
 */
char	*CST_FAIL_TIP = NULL;

/*
 - Shared assertion logic
 */

#define CST_ASSERT(expr, func, errmsg) do {\
	if ((expr)) {\
		CST_FAIL_TIP = NULL;\
		fprintf(stderr, CST_GREEN"✅ %s"CST_RES, CST_TEST_NAME);\
		break ;\
	}\
	fprintf(stderr, CST_RED"❌ %s", CST_TEST_NAME);\
	if (CST_SHOW_FAIL_DETAILS) {\
		fprintf(stderr, CST_GRAY": "CST_RED);\
		errmsg;\
		fprintf(stderr, " from %s", #func);\
	}\
	if (CST_FAIL_TIP != NULL)\
		fprintf(stderr, CST_GRAY" - "CST_RED"%s", CST_FAIL_TIP);\
	fprintf(stderr, CST_RES);\
	CST_FAIL_TIP = NULL;\
	exit(EXIT_FAILURE);\
} while (0)

/*
 - Assertions - NULL */

#define ASSERT_NULL(expr) CST_ASSERT(expr == NULL, expr, fprintf(stderr, "Got NOT NULL when expecting NULL"));

#define ASSERT_NOT_NULL(expr) CST_ASSERT(expr != NULL, expr, fprintf(stderr, "Got NULL when expecting NOT NULL"));


/*
 - Assertions - Bool
 */

/**
 * @brief Asserts that the provided `expr`ession is `true`.
 * If `expr` evaluates to `false`, the test will fail.
 * 
 * @param expr The expression to evaluate (Generally just a function call).
 */
#define ASSERT_TRUE(expr) CST_ASSERT(expr, expr, fprintf(stderr, "Got FALSE when expecting TRUE"));

/**
 * @brief Asserts that the provided `expr`ession is `false`.
 * If `expr` evaluates to `true`, the test will fail.
 * 
 * @param expr The expression to evaluate (Generally just a function call).
 */
#define ASSERT_FALSE(expr) CST_ASSERT(!expr, expr, fprintf(stderr, "Got TRUE when expecting FALSE"));

/*
 - Assertions - Char
 */

#define ASSERT_CHAR_EQUALS(expr, expected) do {\
	char cst_actual = (expr);\
	char cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got '%c' when expecting '%c'", cst_actual, cst_expected));\
} while (0)

#define ASSERT_UCHAR_EQUALS(expr, expected) do {\
	unsigned char cst_actual = (expr);\
	unsigned char cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got '%u' when expecting '%u'", cst_actual, cst_expected));\
} while (0)

/*
 - Assertions - Int
 */

#define ASSERT_INT_EQUALS(expr, expected) do {\
	int cst_actual = (expr);\
	int cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %i when expecting %i", cst_actual, cst_expected));\
} while (0)

#define ASSERT_UINT_EQUALS(expr, expected) do {\
	unsigned int cst_actual = (expr);\
	unsigned int cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %u when expecting %u", cst_actual, cst_expected));\
} while (0)

#endif
