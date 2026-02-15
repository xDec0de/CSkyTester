#include "cst.h"
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <ctype.h>

/*
 - cst_sighandler.c
 */

void	cst_init_sighandler(void);

/*
 - Internal data
 */

typedef struct cst_test
{
	const char		*category;
	const char		*name;
	void			(*func)(void);
	long			timeout;
	bool			executed;
	struct cst_test	*next;
}	cst_test;

typedef struct cst_hook
{
	const char		*category;
	void			(*func)(void);
	struct cst_hook	*next;
}	cst_hook;

static size_t	CST_START_DATE = ULONG_MAX;
static cst_test	*CST_TESTS = NULL;
static cst_hook	*CST_AFTER_ALL = NULL;
static cst_hook	*CST_AFTER_EACH = NULL;
static cst_hook	*CST_BEFORE_ALL = NULL;
static cst_hook	*CST_BEFORE_EACH = NULL;
static bool		CST_MEMCHECK = true;
static bool		CST_SIGHANDLER = true;
static bool		CST_ON_TEST = false;
static long		CST_TIMEOUT_MS = 0;

/*
 - Exposed variables
 */

char	*CST_TEST_NAME			= "";
char	*CST_FAIL_TIP			= NULL;
bool	CST_SHOW_FAIL_DETAILS	= true;
bool	CST_DO_BACKTRACE		= true;

bool	cst_is_on_test(void)
{
	return CST_ON_TEST;
}

/*
 - Program exit util
 */

static cst_hook *cst_free_hook(cst_hook *lst)
{
	if (lst == NULL)
		return (NULL);
	for (cst_hook *hook = lst, *tmp; hook != NULL; hook = tmp) {
		tmp = hook->next;
		free(hook);
	}
	return (NULL);
}

static void cst_free(void)
{
	if (CST_TESTS != NULL) {
		for (cst_test *test = CST_TESTS, *tmp; test != NULL; test = tmp) {
			tmp = test->next;
			free(test);
		}
		CST_TESTS = NULL;
	}
	CST_AFTER_ALL = cst_free_hook(CST_AFTER_ALL);
	CST_AFTER_EACH = cst_free_hook(CST_AFTER_EACH);
	CST_BEFORE_ALL = cst_free_hook(CST_BEFORE_ALL);
	CST_BEFORE_EACH = cst_free_hook(CST_BEFORE_EACH);
}

static void	cst_exit(char *errmsg, int ec)
{
	cst_free();
	if (errmsg != NULL)
		printf(CST_RED"CST Error"CST_GRAY": "CST_BRED"%s"CST_RES"\n", errmsg);
	_exit(ec);
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
		return (ptr);
	cst_exit("Malloc failed", 100);
	return (NULL);
}

/*
 - Test execution
 */

static void cst_run_hook(cst_hook *lst, const char *category)
{
	for (cst_hook *hook = lst; hook != NULL; hook = hook->next) {
		if (category == NULL && hook->category != NULL)
			continue;
		if (hook->category == NULL && category != NULL)
			continue;
		if ((hook->category == NULL && category == NULL) || strcmp(hook->category, category) == 0)
			hook->func();
	}
}

static bool cst_run_test(cst_test *test)
{
	pid_t	pid = fork();

	if (pid == -1)
		cst_exit("Failed to fork", 2);
	if (pid == 0) {
		void (*func)(void) = test->func;
		CST_ON_TEST = true;
		CST_TEST_NAME = (char *) test->name;
		cst_free();
		func();
		fprintf(stderr, CST_GREEN"✅ %s\n"CST_RES, CST_TEST_NAME);
		cst_check_leaks_before_exit();
		_exit(EXIT_SUCCESS);
	} else {
		int ec = 0;
		test->executed = true;
		if (test->timeout < 0)
			test->timeout = CST_TIMEOUT_MS;
		if (test->timeout <= 0) {
			waitpid(pid, &ec, 0);
			return (ec == 0);
		}
		size_t start = cst_now_ms();
		while (true) {
			pid_t res = waitpid(pid, &ec, WNOHANG);
			if (res == -1)
				cst_exit("waitpid failed", 3);
			else if (res > 0)
				return (ec == 0);
			if ((cst_now_ms() - start) >= (size_t) test->timeout) {
				kill(pid, SIGKILL);
				waitpid(pid, &ec, 0);
				printf(CST_BRED"❌ %s "CST_GRAY"-"CST_RED" Timed out (%ld ms)\n"CST_RES, test->name, test->timeout);
				return false;
			}
			usleep(50);
		}
	}
}

static void cst_run_test_category(const char *name, size_t *failed)
{
	printf("\n");
	cst_run_hook(CST_BEFORE_ALL, name);
	if (name[0] != '\0')
		printf(CST_BBLUE "%s" CST_GRAY ":" CST_RES "\n", name);
	for (cst_test *test = CST_TESTS; test != NULL; test = test->next) {
		if (strcmp(name, test->category) == 0) {
			cst_run_hook(CST_BEFORE_EACH, name);
			cst_run_hook(CST_BEFORE_EACH, NULL);
			if (!cst_run_test(test))
				(*failed)++;
			cst_run_hook(CST_AFTER_EACH, name);
			cst_run_hook(CST_AFTER_EACH, NULL);
		}
	}
	cst_run_hook(CST_AFTER_ALL, name);
}

static int	cst_run_tests()
{
	size_t	failed = 0;
	size_t	total = 0;

	cst_run_hook(CST_BEFORE_ALL, NULL);
	for (cst_test *tmp = CST_TESTS; tmp != NULL; tmp = tmp->next)
		total++;
	cst_run_test_category("", &failed);
	for (cst_test *test = CST_TESTS; test != NULL; test = test->next)
		if (!test->executed)
			cst_run_test_category(test->category, &failed);
	cst_run_hook(CST_AFTER_ALL, NULL);
	if (failed == 0)
		printf(CST_BGREEN "\n✅ All %zu tests passed!", total);
	else
		printf(CST_BRED "\n❌ Failed " CST_BYELLOW "%zu" CST_GRAY "/" CST_YELLOW "%zu" CST_BRED " test(s)", failed, total);
	printf(CST_GRAY " - " CST_YELLOW "%zums" CST_RES "\n", (cst_now_ms() - CST_START_DATE));
	return (failed);
}

/*
 - Hook registration
 */

static cst_hook *cst_register_hook(cst_hook *lst, const char *category, void (*func)(void))
{
	cst_hook	*fixture;

	fixture = cst_malloc(sizeof(cst_hook));
	fixture->category = category;
	fixture->func = func;
	fixture->next = NULL;
	if (lst == NULL)
		return (fixture);
	else {
		for (cst_hook *tmp = lst; true; tmp = tmp->next) {
			if (tmp->next == NULL) {
				tmp->next = fixture;
				break;
			}
		}
		return (lst);
	}
}

void cst_register_after_all(const char *category, void (*func)(void))
{
	CST_AFTER_ALL = cst_register_hook(CST_AFTER_ALL, category, func);
}

void cst_register_after_each(const char *category, void (*func)(void))
{
	CST_AFTER_EACH = cst_register_hook(CST_AFTER_EACH, category, func);
}

void cst_register_before_all(const char *category, void (*func)(void))
{
	CST_BEFORE_ALL = cst_register_hook(CST_BEFORE_ALL, category, func);
}

void cst_register_before_each(const char *category, void (*func)(void))
{
	CST_BEFORE_EACH = cst_register_hook(CST_BEFORE_EACH, category, func);
}

/*
 - Test registration
 */

void cst_register_test(const char *category, const char *name, long timeout, void (*func)(void))
{
	cst_test	*test;

	test = cst_malloc(sizeof(cst_test));
	test->category = category == NULL ? "" : category;
	test->name = name == NULL ? "???" : name;
	test->timeout = timeout;
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

static void get_timeout(const char *timeout)
{
	long	ms = atol(timeout);

	for (size_t i = 0; timeout[i] != '\0'; i++)
		if (!isdigit(timeout[i]))
			cst_exit("Invalid -timeout value. Zero or a positive number is required", 1);
	CST_TIMEOUT_MS = ms < 0 ? 0 : ms;
}

int main(int argc, char **argv)
{
	CST_START_DATE = cst_now_ms();
	if (CST_TESTS == NULL)
		cst_exit("No tests to run", 1);
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (strcmp(arg, "-nomem") == 0 || strcmp(arg, "-nomemcheck") == 0)
			CST_MEMCHECK = false;
		else if (strcmp(arg, "-nobt") == 0 || strcmp(arg, "-nobacktrace") == 0)
			CST_DO_BACKTRACE = false;
		else if (strcmp(arg, "-nosig") == 0 || strcmp(arg, "-nosighandler") == 0)
			CST_SIGHANDLER = false;
		else if (strcmp(arg, "-timeout=") == 0)
			get_timeout(arg + 9);
		else
			printf(CST_GRAY"["CST_BYELLOW"CST"CST_GRAY"] "CST_YELLOW"Ignored unknown argument"CST_GRAY": "CST_BYELLOW"%s"CST_RES"\n", arg);
	}
	if (CST_SIGHANDLER)
		cst_init_sighandler();
	cst_exit(NULL, cst_run_tests());
}
