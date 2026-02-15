#ifndef CST_H
# define CST_H

/* stdout & fprintf */
#include <stdio.h>

/* exit */
#include <stdlib.h>

/* bool type */
#include <stdbool.h>

/* fabsf, fabs & fabsl */
#include <math.h>

#define CST_VERSION "1.0.0"

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

extern bool	CST_DO_BACKTRACE;

/*
 - Backtraces
 */

#ifndef CST_MAX_BT
# define CST_MAX_BT 32
#endif

#ifndef CST_PATH_MAX
# define CST_PATH_MAX 1024
#endif

void cst_bt_print_current(int skip);

/*
 - Memory checking
 */

void* cst_malloc_impl(size_t size, const char *file, int line);
void* cst_calloc_impl(size_t nmemb, size_t size, const char *file, int line);
void* cst_realloc_impl(void *ptr, size_t size, const char *file, int line);
void cst_free_impl(void *ptr, const char *file, int line);

// Manual leak checking
bool cst_has_leaks(void);
void cst_print_leaks(void);
void cst_reset_memcheck(void);
void cst_check_leaks_before_exit(void);

// Override malloc/free with tracking (unless disabled)
#ifndef CST_NO_MEMCHECK
# define malloc(size) cst_malloc_impl(size, __FILE__, __LINE__)
# define calloc(nmemb, size) cst_calloc_impl(nmemb, size, __FILE__, __LINE__)
# define realloc(ptr, size) cst_realloc_impl(ptr, size, __FILE__, __LINE__)
# define free(ptr) cst_free_impl(ptr, __FILE__, __LINE__)
#endif

/*
 - Test registration
 */

void cst_register_test(const char *category, const char *name, long timeout, void (*func)(void));

#define __CST_STRCAT_IMPL(a,b) a##b
#define __CST_STRCAT(a,b) __CST_STRCAT_IMPL(a,b)

#define __CST_GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define __CST_TEST_IMPL(CAT, NAME, TIMEOUT, ID) \
	static void __CST_STRCAT(__cst_fn_, ID)(void); \
	static void __attribute__((constructor)) \
	__CST_STRCAT(__cst_ctor_, ID)(void) { \
		cst_register_test((CAT), (NAME), (TIMEOUT), __CST_STRCAT(__cst_fn_, ID)); \
	} \
	static void __CST_STRCAT(__cst_fn_, ID)(void)

#define __CST_TEST2(CAT, NAME) \
	__CST_TEST_IMPL((CAT), (NAME), -1, __COUNTER__)

#define __CST_TEST3(CAT, NAME, TIMEOUT) \
	__CST_TEST_IMPL((CAT), (NAME), (TIMEOUT), __COUNTER__)

#define TEST(...) __CST_GET_MACRO(__VA_ARGS__, __CST_TEST3, __CST_TEST2)(__VA_ARGS__)

/*
 - Hooks - Before all
 */

void cst_register_before_all(const char *category, void (*func)(void));

#define __CST_BEFORE_ALL_IMPL(category, id) \
	static void __CST_STRCAT(__cst_fn_, id)(void); \
	static void __attribute__((constructor)) \
	__CST_STRCAT(__cst_ctor_, id)(void) { \
		cst_register_before_all((category), __CST_STRCAT(__cst_fn_, id)); \
	} \
	static void __CST_STRCAT(__cst_fn_, id)(void)

#define CST_BEFORE_ALL(category) __CST_BEFORE_ALL_IMPL((category), __COUNTER__)

/*
 - Hooks - Before each
 */

void cst_register_before_each(const char *category, void (*func)(void));

#define __CST_BEFORE_EACH_IMPL(category, id) \
	static void __CST_STRCAT(__cst_fn_, id)(void); \
	static void __attribute__((constructor)) \
	__CST_STRCAT(__cst_ctor_, id)(void) { \
		cst_register_before_each((category), __CST_STRCAT(__cst_fn_, id)); \
	} \
	static void __CST_STRCAT(__cst_fn_, id)(void)

#define CST_BEFORE_EACH(category) __CST_BEFORE_EACH_IMPL((category), __COUNTER__)

/*
 - Hooks - After all
 */

void cst_register_after_all(const char *category, void (*func)(void));

#define __CST_AFTER_ALL_IMPL(category, id) \
	static void __CST_STRCAT(__cst_fn_, id)(void); \
	static void __attribute__((constructor)) \
	__CST_STRCAT(__cst_ctor_, id)(void) { \
		cst_register_after_all((category), __CST_STRCAT(__cst_fn_, id)); \
	} \
	static void __CST_STRCAT(__cst_fn_, id)(void)

#define CST_AFTER_ALL(category) __CST_AFTER_ALL_IMPL((category), __COUNTER__)

/*
 - Hooks - After each
 */

void cst_register_after_each(const char *category, void (*func)(void));

#define __CST_AFTER_EACH_IMPL(category, id) \
	static void __CST_STRCAT(__cst_fn_, id)(void); \
	static void __attribute__((constructor)) \
	__CST_STRCAT(__cst_ctor_, id)(void) { \
		cst_register_after_each((category), __CST_STRCAT(__cst_fn_, id)); \
	} \
	static void __CST_STRCAT(__cst_fn_, id)(void)

#define CST_AFTER_EACH(category) __CST_AFTER_EACH_IMPL((category), __COUNTER__)

/*
 - Shared assertion logic
 */

#define CST_ASSERT(expr, func, errmsg) do {\
	if ((expr)) {\
		CST_FAIL_TIP = NULL;\
		cst_check_leaks_before_exit();\
		fprintf(stderr, CST_GREEN"✅ %s\n"CST_RES, CST_TEST_NAME);\
		exit(EXIT_SUCCESS);\
	}\
	fprintf(stderr, CST_BRED"❌ %s"CST_RED, CST_TEST_NAME);\
	if (CST_SHOW_FAIL_DETAILS) {\
		fprintf(stderr, CST_GRAY": "CST_RED);\
		errmsg;\
		fprintf(stderr, " from %s", #func);\
	}\
	if (CST_FAIL_TIP != NULL)\
		fprintf(stderr, CST_GRAY" - "CST_RED"%s", CST_FAIL_TIP);\
	fprintf(stderr, "\n"CST_RES);\
	CST_FAIL_TIP = NULL;\
	cst_check_leaks_before_exit();\
	exit(EXIT_FAILURE);\
} while (0)

#define CST_ASSERT_FREE(ptr, expr, func, errmsg) do {\
	if ((expr)) {\
		CST_FAIL_TIP = NULL;\
		free((ptr));\
		cst_check_leaks_before_exit();\
		fprintf(stderr, CST_GREEN"✅ %s\n"CST_RES, CST_TEST_NAME);\
		exit(EXIT_SUCCESS);\
	}\
	fprintf(stderr, CST_BRED"❌ %s"CST_RED, CST_TEST_NAME);\
	if (CST_SHOW_FAIL_DETAILS) {\
		fprintf(stderr, CST_GRAY": "CST_RED);\
		errmsg;\
		fprintf(stderr, " from %s", #func);\
	}\
	if (CST_FAIL_TIP != NULL)\
		fprintf(stderr, CST_GRAY" - "CST_RED"%s", CST_FAIL_TIP);\
	fprintf(stderr, "\n"CST_RES);\
	CST_FAIL_TIP = NULL;\
	free((ptr));\
	cst_check_leaks_before_exit();\
	exit(EXIT_FAILURE);\
} while (0)

/*
 - Utils - Strings
 */

bool cst_str_equals(const char *s1, const char *s2);

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

/*
 - Assertions - String
 */

#define ASSERT_STR_EQUALS(expr, expected) do {\
	char *cst_actual = (expr);\
	char *cst_expected = (expected);\
	CST_ASSERT(cst_str_equals(cst_actual, cst_expected), expr, fprintf(stderr, "Got \"%s\" when expecting \"%s\"", cst_actual, cst_expected));\
} while (0)

#define ASSERT_STR_EQUALS_FREE(expr, expected) do {\
	char *cst_actual = (expr);\
	char *cst_expected = (expected);\
	bool cst_result = cst_str_equals(cst_actual, cst_expected);\
	CST_ASSERT_FREE(cst_actual, cst_result, expr, fprintf(stderr, "Got \"%s\" when expecting \"%s\"", cst_actual, cst_expected));\
} while (0)

#define ASSERT_STR_NOT_EQUALS(expr, expected) do {\
	char *cst_actual = (expr);\
	char *cst_expected = (expected);\
	CST_ASSERT(!(cst_str_equals(cst_actual, cst_expected)), expr, fprintf(stderr, "Got \"%s\" when expecting NOT \"%s\"", cst_actual, cst_expected));\
} while (0)

#define ASSERT_STR_NOT_EQUALS_FREE(expr, expected) do {\
	char *cst_actual = (expr);\
	char *cst_expected = (expected);\
	bool cst_result = !cst_str_equals(cst_actual, cst_expected);\
	CST_ASSERT_FREE(cst_actual, cst_result, expr, fprintf(stderr, "Got \"%s\" when expecting NOT \"%s\"", cst_actual, cst_expected));\
} while (0)

/*
 - Colors
 */

# define CST_RED		"\033[0;31m"
# define CST_BRED		"\033[1;31m"
# define CST_GREEN		"\033[0;32m"
# define CST_BGREEN		"\033[1;32m"
# define CST_YELLOW		"\033[0;33m"
# define CST_BYELLOW	"\033[1;33m"
# define CST_BLUE		"\033[0;36m"
# define CST_BBLUE		"\033[1;36m"
# define CST_WHITE		"\033[0;37m"
# define CST_BWHITE		"\033[1;37m"
# define CST_GRAY		"\033[0;30m"
# define CST_RES		"\033[0m"

#endif
