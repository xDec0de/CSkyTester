
#include "cst_color.h"
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct cst_test
{
	char			*category;
	char			*name;
	void			(*func)(void);
	bool			executed;
	struct cst_test	*next;
}	cst_test;

static size_t		CST_START_DATE = ULONG_MAX;
static cst_test		*CST_TESTS = NULL;
static bool			CST_MEMCHECK = true;
static bool			CST_SIGHANDLER = true;

/*
 - Test configuration
 */

char	*CST_TEST_NAME			= "";
char	*CST_FAIL_TIP			= NULL;
bool	CST_SHOW_FAIL_DETAILS	= true;

/*
 - Program exit util
 */

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
		printf(CST_RED"CST Error"CST_GRAY": "CST_BRED"%s"CST_RES"\n", errmsg);
	exit(ec);
}

/*
 - Signal handler
 */

char *cst_getsigname(int signum)
{
	if (signum == SIGABRT)
		return ("SIGABRT");
	if (signum == SIGFPE)
		return ("SIGFPE");
	if (signum == SIGILL)
		return ("SIGILL");
	if (signum == SIGINT)
		return ("SIGINT");
	if (signum == SIGSEGV)
		return ("SIGSEGV / Segmentation fault");
	if (signum == SIGTERM)
		return ("SIGTERM");
	if (signum == SIGBUS)
		return ("SIGBUS");
	if (signum == SIGQUIT)
		return ("SIGQUIT");
	if (signum == SIGHUP)
		return ("SIGHUP");
	return ("???");
}

void cst_sighandler(int signum)
{
	fprintf(stderr, CST_BRED"💥 %s "CST_GRAY"-"CST_RED" Crashed with signal %i (%s)\n"CST_RES,
		CST_TEST_NAME, signum, cst_getsigname(signum));
	cst_exit(NULL, EXIT_FAILURE);
}

static void cst_init_sighandler(void)
{
	static bool handling = false;

	if (handling)
		return;
	handling = true;

	// Crash signals
	signal(SIGABRT, cst_sighandler);
	signal(SIGFPE,  cst_sighandler);
	signal(SIGILL,  cst_sighandler);
	signal(SIGSEGV, cst_sighandler);
	signal(SIGBUS,  cst_sighandler);

	// Interruptions / terminations
	signal(SIGINT,  cst_sighandler);
	signal(SIGTERM, cst_sighandler);
	signal(SIGQUIT, cst_sighandler);
	signal(SIGHUP,  cst_sighandler);
}

/*
 - Time getter
 */

static size_t cst_now_ms(void)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
		return (size_t) - 1;
	return ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
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
	cst_exit("Malloc failed", 100);
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
	size_t	failed = 0;
	size_t	total = 0;

	for (cst_test *tmp = CST_TESTS; tmp != NULL; tmp = tmp->next)
		remaining++;
	total = remaining;
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
		printf(CST_BGREEN "\n✅ All %zu tests passed!", total);
	else
		printf(CST_BRED "\n❌ Failed " CST_BYELLOW "%zu" CST_GRAY "/" CST_YELLOW "%zu" CST_BRED " test(s)", failed, total);
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

/*
 - Program entry point
 */

int main(int argc, char **argv)
{
	CST_START_DATE = cst_now_ms();
	if (CST_TESTS == NULL)
		cst_exit("No tests to run", 1);
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (strcmp(arg, "-nomem") == 0 || strcmp(arg, "-nomemcheck") == 0)
			CST_MEMCHECK = false;
		else if (strcmp(arg, "-nosig") == 0 || strcmp(arg, "-nosighandler") == 0)
			CST_SIGHANDLER = false;
	}
	if (CST_SIGHANDLER)
		cst_init_sighandler();
	cst_run_tests();
	cst_exit(NULL, EXIT_SUCCESS);
}
