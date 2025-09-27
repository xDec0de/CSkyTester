#ifndef CST_ASSERTIONS_H
# define CST_ASSERTIONS_H

#include "cst_color.h"

/* stdout & fprintf */
#include <stdio.h>

/* exit */
#include <stdlib.h>

/* bool type */
#include <stdbool.h>

/* fabsf, fabs & fabsl */
#include <math.h>

/* Custom test name macros with fallback */

#ifndef CST_FILE
# define CST_FILE "???"
#endif

/* Global variables for configuration */

extern char	*CST_TEST_NAME;

/**
 * @brief Whether to display the default assertion failure details.
 * If `true`, whenever an assertion fails, a default description
 * such as "Got TRUE when expecting FALSE from (function)" is displayed.
 * 
 * Default: `true`
 */
extern bool	CST_SHOW_FAIL_DETAILS;

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
extern char	*CST_FAIL_TIP;

/*
 - Test registration
 */

void cst_register_test(const char *category, const char *name, void (*func)(void));

#define __CST_STRCAT_IMPL(a,b) a##b
#define __CST_STRCAT(a,b)  __CST_STRCAT_IMPL(a,b)

#define __CST_TEST_IMPL(CAT, NAME, ID) \
	static void __CST_STRCAT(__cst_fn_, ID)(void); \
	static void __attribute__((constructor)) \
	__CST_STRCAT(__cst_ctor_, ID)(void) { \
		cst_register_test((CAT), (NAME), __CST_STRCAT(__cst_fn_, ID)); \
	} \
	static void __CST_STRCAT(__cst_fn_, ID)(void)

#define TEST(category, name) __CST_TEST_IMPL((category), (name), __COUNTER__)

/*
 - Shared assertion logic
 */

#define CST_ASSERT(expr, func, errmsg) do {\
	if ((expr)) {\
		CST_FAIL_TIP = NULL;\
		fprintf(stderr, CST_GREEN"✅ %s"CST_RES, CST_TEST_NAME);\
		break ;\
	}\
	fprintf(stderr, CST_BRED"❌ %s"CST_RED, CST_TEST_NAME);\
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
 - Assertions - NULL
 */

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

#define ASSERT_CHAR_NOT_EQUALS(expr, expected) do {\
	char cst_actual = (expr);\
	char cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got '%c' when expecting NOT '%c'", cst_actual, cst_expected));\
} while (0)

#define ASSERT_UCHAR_EQUALS(expr, expected) do {\
	unsigned char cst_actual = (expr);\
	unsigned char cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got '%u' when expecting '%u'", cst_actual, cst_expected));\
} while (0)

#define ASSERT_UCHAR_NOT_EQUALS(expr, expected) do {\
	unsigned char cst_actual = (expr);\
	unsigned char cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got '%u' when expecting NOT '%u'", cst_actual, cst_expected));\
} while (0)

/*
 - Assertions - Int
 */

#define ASSERT_INT_EQUALS(expr, expected) do {\
	int cst_actual = (expr);\
	int cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %i when expecting %i", cst_actual, cst_expected));\
} while (0)

#define ASSERT_INT_NOT_EQUALS(expr, expected) do {\
	int cst_actual = (expr);\
	int cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %i when expecting NOT %i", cst_actual, cst_expected));\
} while (0)

#define ASSERT_UINT_EQUALS(expr, expected) do {\
	unsigned int cst_actual = (expr);\
	unsigned int cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %u when expecting %u", cst_actual, cst_expected));\
} while (0)

#define ASSERT_UINT_NOT_EQUALS(expr, expected) do {\
	unsigned int cst_actual = (expr);\
	unsigned int cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %u when expecting NOT %u", cst_actual, cst_expected));\
} while (0)

/*
 - Assertions - Long
 */

#define ASSERT_LONG_EQUALS(expr, expected) do {\
	long cst_actual = (expr);\
	long cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %ld when expecting %ld", cst_actual, cst_expected));\
} while (0)

#define ASSERT_LONG_NOT_EQUALS(expr, expected) do {\
	long cst_actual = (expr);\
	long cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %ld when expecting NOT %ld", cst_actual, cst_expected));\
} while (0)

#define ASSERT_ULONG_EQUALS(expr, expected) do {\
	unsigned long cst_actual = (expr);\
	unsigned long cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %lu when expecting %lu", cst_actual, cst_expected));\
} while (0)

#define ASSERT_ULONG_NOT_EQUALS(expr, expected) do {\
	unsigned long cst_actual = (expr);\
	unsigned long cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %lu when expecting NOT %lu", cst_actual, cst_expected));\
} while (0)

/*
 - Assertions - Long long
 */

#define ASSERT_LLONG_EQUALS(expr, expected) do {\
	long long cst_actual = (expr);\
	long long cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %lld when expecting %lld", cst_actual, cst_expected));\
} while (0)

#define ASSERT_LLONG_NOT_EQUALS(expr, expected) do {\
	long long cst_actual = (expr);\
	long long cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %lld when expecting NOT %lld", cst_actual, cst_expected));\
} while (0)

#define ASSERT_ULLONG_EQUALS(expr, expected) do {\
	unsigned long long cst_actual = (expr);\
	unsigned long long cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %llu when expecting %llu", cst_actual, cst_expected));\
} while (0)

#define ASSERT_ULLONG_NOT_EQUALS(expr, expected) do {\
	unsigned long long cst_actual = (expr);\
	unsigned long long cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %llu when expecting NOT %llu", cst_actual, cst_expected));\
} while (0)

/*
 - Assertions - Short
 */

#define ASSERT_SHORT_EQUALS(expr, expected) do {\
	short cst_actual = (expr);\
	short cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %hd when expecting %hd", cst_actual, cst_expected));\
} while (0)

#define ASSERT_SHORT_NOT_EQUALS(expr, expected) do {\
	short cst_actual = (expr);\
	short cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %hd when expecting NOT %hd", cst_actual, cst_expected));\
} while (0)

#define ASSERT_USHORT_EQUALS(expr, expected) do {\
	unsigned short cst_actual = (expr);\
	unsigned short cst_expected = (expected);\
	CST_ASSERT(cst_actual == cst_expected, expr, fprintf(stderr, "Got %hu when expecting %hu", cst_actual, cst_expected));\
} while (0)

#define ASSERT_USHORT_NOT_EQUALS(expr, expected) do {\
	unsigned short cst_actual = (expr);\
	unsigned short cst_expected = (expected);\
	CST_ASSERT(cst_actual != cst_expected, expr, fprintf(stderr, "Got %hu when expecting NOT %hu", cst_actual, cst_expected));\
} while (0)

/*
 - Assertions - Float
 */

#define ASSERT_FLOAT_EQUALS_APPROX(expr, expected, tol) do {\
	float cst_actual = (expr);\
	float cst_expected = (expected);\
	CST_ASSERT(fabsf(cst_actual - cst_expected) <= (tol), expr, \
		fprintf(stderr, "Got %f when expecting %f ± %f", cst_actual, cst_expected, tol));\
} while (0)

#define ASSERT_FLOAT_EQUALS(expr, expected) ASSERT_FLOAT_EQUALS_APPROX((expr), (expected), 1e-6f)

#define ASSERT_FLOAT_NOT_EQUALS_APPROX(expr, expected, tol) do {\
	float cst_actual = (expr);\
	float cst_expected = (expected);\
	CST_ASSERT(!(fabsf(cst_actual - cst_expected) <= (tol)), expr, \
		fprintf(stderr, "Got %f when expecting NOT %f ± %f", cst_actual, cst_expected, tol));\
} while (0)

#define ASSERT_FLOAT_NOT_EQUALS(expr, expected) ASSERT_FLOAT_NOT_EQUALS_APPROX((expr), (expected), 1e-6f)

/*
 - Assertions - Double
 */

#define ASSERT_DOUBLE_EQUALS_APPROX(expr, expected, tol) do {\
	double cst_actual = (expr);\
	double cst_expected = (expected);\
	CST_ASSERT(fabs(cst_actual - cst_expected) <= (tol), expr, \
		fprintf(stderr, "Got %lf when expecting %lf ± %lf", cst_actual, cst_expected, tol));\
} while (0)

#define ASSERT_DOUBLE_EQUALS(expr, expected) ASSERT_DOUBLE_EQUALS_APPROX((expr), (expected), 1e-12)

#define ASSERT_DOUBLE_NOT_EQUALS_APPROX(expr, expected, tol) do {\
	double cst_actual = (expr);\
	double cst_expected = (expected);\
	CST_ASSERT(!(fabs(cst_actual - cst_expected) <= (tol)), expr, \
		fprintf(stderr, "Got %lf when expecting %lf ± %lf", cst_actual, cst_expected, tol));\
} while (0)

#define ASSERT_DOUBLE_NOT_EQUALS(expr, expected) ASSERT_DOUBLE_NOT_EQUALS_APPROX((expr), (expected), 1e-12)

/*
 - Assertions - Long double
 */

#define ASSERT_LDOUBLE_EQUALS_APPROX(expr, expected, tol) do {\
	long double cst_actual = (expr);\
	long double cst_expected = (expected);\
	CST_ASSERT(fabsl(cst_actual - cst_expected) <= (tol), expr, \
		fprintf(stderr, "Got %Lf when expecting %Lf ± %Lf", cst_actual, cst_expected, tol));\
} while (0)

#define ASSERT_LDOUBLE_EQUALS(expr, expected) ASSERT_LDOUBLE_EQUALS_APPROX((expr), (expected), 1e-15L)

#define ASSERT_LDOUBLE_NOT_EQUALS_APPROX(expr, expected, tol) do {\
	long double cst_actual = (expr);\
	long double cst_expected = (expected);\
	CST_ASSERT(!(fabsl(cst_actual - cst_expected) <= (tol)), expr, \
		fprintf(stderr, "Got %Lf when expecting %Lf ± %Lf", cst_actual, cst_expected, tol));\
} while (0)

#define ASSERT_LDOUBLE_NOT_EQUALS(expr, expected) ASSERT_LDOUBLE_NOT_EQUALS_APPROX((expr), (expected), 1e-15L)

#endif
