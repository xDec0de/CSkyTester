
#include "internal/cst_internals.h"
#include "cst_color.h"
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/wait.h>

static size_t		CST_START_DATE = ULONG_MAX;
static cst_args		CST_ARGS;
static cst_test		*CST_TESTS = NULL;

/*
 - Test configuration
 */

char	*CST_TEST_NAME			= "";
char	*CST_FAIL_TIP			= NULL;
bool	CST_SHOW_FAIL_DETAILS	= true;

/*
 - Program exit util
 */

static size_t cst_now_ms(void)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
		return (size_t) - 1;
	return ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
}

static void	cst_exit(char *errmsg, int ec)
{
	if (CST_TESTS != NULL) {
		for (cst_test *test = CST_TESTS, *tmp; test != NULL; test = tmp) {
			tmp = test->next;
			free(test);
		}
		CST_TESTS = NULL;
	}
	if (errmsg != NULL)
		printf(CST_ERR_PREFIX"%s"CST_RES"\n", errmsg);
	exit(ec);
}

/*
 - Malloc util
 */

static void	*cst_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (ptr != NULL)
		return ptr;
	cst_exit(ALLOC_FAIL_ERR, ALLOC_FAIL_ERRC);
}

/*
 - Test execution
 */

static bool cst_run_test(cst_test *test)
{
	char	*cmd;
	pid_t	pid = fork();

	if (pid == -1)
		cst_exit("Failed to fork", 2);
	if (pid == 0) {
		CST_TEST_NAME = test->name;
		test->func();
		cst_exit(NULL, EXIT_SUCCESS);
	} else {
		int	ec = 0;
		waitpid(pid, &ec, 0);
		test->executed = true;
		return (ec == 0);
	}
}

static void	cst_run_tests()
{
	char	*cat = NULL;
	size_t	remaining = 0;
	int		failed = 0;

	for (cst_test *tmp = CST_TESTS; tmp != NULL; tmp = tmp->next)
		remaining++;
	while (remaining != 0) {
		cat = NULL;
		for (cst_test *test = CST_TESTS; test != NULL; test = test->next) {
			if (test->executed)
				continue;
			if (cat == NULL) {
				cat = test->category;
				printf(CST_BBLUE "\n%s" CST_GRAY ":" CST_RES "\n", cat);
				if (!cst_run_test(test))
					failed++;
				remaining--;
			} else if (strcmp(cat, test->category) == 0) {
				if (!cst_run_test(test))
					failed++;
				remaining--;
			}
		}
	}
	if (failed == 0)
		printf(CST_BGREEN "\n✅ All tests passed!");
	else
		printf(CST_BRED "\n❌ Failed " CST_BYELLOW "%d" CST_BRED " test(s)", failed);
	printf(CST_GRAY " - " CST_YELLOW "%zums" CST_RES "\n", (cst_now_ms() - CST_START_DATE));
}

/*
 - Test registration
 */

void cst_register_test(const char *category, const char *name, void (*func)(void))
{
	cst_test	*test;

	test = cst_malloc(sizeof(cst_test));
	test->category = category;
	test->name = name;
	test->func = func;
	test->executed = false;
	test->next = NULL;
	if (CST_TESTS == NULL)
		CST_TESTS = test;
	else {
		for (cst_test *tmp = CST_TESTS; true; tmp = tmp->next) {
			if (tmp->next == NULL) {
				tmp->next = test;
				break;
			}
		}
	}
}

static void cst_init_args(int argc, char **argv)
{
	CST_ARGS.memcheck = true;
	CST_ARGS.sighandler = true;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (strcmp(arg, "-nomem") == 0 || strcmp(arg, "-nomemcheck") == 0)
			CST_ARGS.memcheck = false;
		else if (strcmp(arg, "-nosig") == 0 || strcmp(arg, "-nosighandler") == 0)
			CST_ARGS.sighandler = false;
	}
}

/*
 - Program entry point
 */

int main(int argc, char **argv)
{
	CST_START_DATE = cst_now_ms();
	if (CST_TESTS == NULL)
		cst_exit("No tests to run", 1);
	cst_init_args(argc, argv);
	cst_run_tests();
	cst_exit(NULL, EXIT_SUCCESS);
}
