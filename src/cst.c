
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

static bool			CST_DEBUG = false;

static size_t		CST_START_DATE = ULONG_MAX;
static char			*CST_DIR = NULL;
static cst_args		CST_ARGS;

static cst_test		*CST_TESTS = NULL;

/*
 - Message utility
 */

static void cst_vdebug(const char *msg, ...)
{
	va_list	args;

	if (!CST_DEBUG)
		return ;
	va_start(args, msg);
	printf(CST_GRAY "[" CST_BWHITE "CST DEBUG" CST_GRAY "] "CST_WHITE);
	vprintf(msg, args);
	va_end(args);
	printf(CST_RES "\n");
}

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
	if (CST_ARGS.test_objs != NULL)
		free(CST_ARGS.test_objs);
	if (CST_ARGS.proj_objs != NULL)
		free(CST_ARGS.proj_objs);
	if (CST_ARGS.extra_flags != NULL)
		free(CST_ARGS.extra_flags);
	if (CST_DIR != NULL)
		free(CST_DIR);
	if (CST_TESTS != NULL) {
		for (cst_test *test = CST_TESTS, *tmp; test != NULL; test = tmp) {
			tmp = test->next;
			if (test->dir != NULL)
				free(test->dir);
			if (test->obj != NULL)
				free(test->obj);
			free(test);
		}
		CST_TESTS = NULL;
	}
	if (errmsg != NULL)
		printf(CST_ERR_PREFIX"%s"CST_RES"\n", errmsg);
	cst_vdebug(CST_BBLUE"Time elapsed"CST_GRAY": "CST_BYELLOW"%zums", cst_now_ms() - CST_START_DATE);
	exit(ec);
}

/*
 - String format util
 */

static char *cst_fmt(const char *fmt, ...)
{
	va_list	tmp, args;
	int		size;
	char	*buf;

	va_start(tmp, fmt);
	va_copy(args, tmp);
	size = vsnprintf(NULL, 0, fmt, tmp);
	va_end(tmp);
	if (size < 0)
		return (va_end(args), NULL);
	buf = malloc((size + 1) * sizeof(char));
	if (buf == NULL)
		return (va_end(args), NULL);
	vsnprintf(buf, size + 1, fmt, args);
	va_end(args);
	return (buf);
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

static void cst_run_test(cst_test *test)
{
	cst_vdebug("- Test: %s", test->obj);
	test->executed = true;
}

static void	cst_run_tests()
{
	char	*dir = NULL;
	size_t	remaining = 0;

	for (cst_test *tmp = CST_TESTS; tmp != NULL; tmp = tmp->next)
		remaining++;
	while (remaining != 0) {
		dir = NULL;
		for (cst_test *test = CST_TESTS; test != NULL; test = test->next) {
			if (test->executed)
				continue;
			if (dir == NULL) {
				dir = test->dir;
				cst_vdebug("Category: %s", dir);
				cst_run_test(test);
				remaining--;
			} else if (strcmp(dir, test->dir) == 0) {
				cst_run_test(test);
				remaining--;
			}
		}
	}
}

/*
 - Prepare test categories
 */

static void	cst_store_test(const char *dir, const char *obj)
{
	cst_test	*test;

	test = cst_malloc(sizeof(cst_test));
	test->dir = dir;
	test->obj = obj;
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

static void cst_tokenize_test(size_t from, size_t to)
{
	const size_t	size = to - from;
	size_t			sep;
	char			*dir;
	char			*obj;

	obj = cst_malloc((size + 1) * sizeof(char));
	for (size_t i = from; i <= to; i++)
		obj[i - from] = CST_ARGS.test_objs[i];
	obj[size] = '\0';
	for (sep = to; sep != from && CST_ARGS.test_objs[sep] != '/'; sep--)
		;
	if (sep == from)
		dir = NULL;
	else {
		dir = cst_malloc((sep - from + 1) * sizeof(char));
		for (size_t i = from; i <= sep; i++)
			dir[i - from] = CST_ARGS.test_objs[i];
		dir[sep - from] = '\0';
	}
	cst_store_test(dir, obj);
}

static void cst_tokenize_tests(void)
{
	size_t	i = 0;
	size_t	last_i = 0;
	char	ch;

	for (; (ch = CST_ARGS.test_objs[i]) != '\0'; i++) {
		if (ch == ' ') {
			cst_tokenize_test(last_i, i);
			last_i = i + 1;
		}
	}
	cst_tokenize_test(last_i, i);
	free(CST_ARGS.test_objs);
	CST_ARGS.test_objs = NULL;
}

/* 
 - Internal sources compilation
 */

static bool cst_is_up_to_date(const char *src, const char *obj)
{
	struct stat st_src, st_obj;

	if (stat(obj, &st_obj) == -1)
		return false;
	if (stat(src, &st_src) == -1)
		return false;
	return st_obj.st_mtime >= st_src.st_mtime;
}

static void cst_compile_internal(const char *file)
{
	char *src = cst_fmt("%s/%s.c", CST_DIR, file);
	char *obj = cst_fmt("%s/objs/%s.o", CST_DIR, file);

	if (cst_is_up_to_date(src, obj)) {
		cst_vdebug(CST_BBLUE "Up to date" CST_GRAY ": " CST_YELLOW "%s", obj);
		free(src); free(obj);
		return;
	}
	char *cmd = cst_fmt("gcc -c %s -o %s", src, obj);
	cst_vdebug(CST_BBLUE "Compiling" CST_GRAY ": " CST_YELLOW "%s", src);
	if (system(cmd) != 0) {
		free(src); free(obj); free(cmd);
		cst_exit(CST_COMPILE_INTERNAL_ERR, CST_COMPILE_INTERNAL_ERRC);
	}
	free(src); free(obj); free(cmd);
}

static void cst_compile_internals(void)
{
	char	*buf;
	size_t	compile_ms = cst_now_ms();

	buf = cst_fmt("%s/objs", CST_DIR);
	if (mkdir(buf, 0777) != 0 && errno != EEXIST) {
		free(buf);
		cst_exit("Failed to create \"objs\" directory", 3);
	}
	free(buf);
	buf = cst_fmt("%s/objs/internal", CST_DIR);
	if (mkdir(buf, 0777) != 0 && errno != EEXIST) {
		free(buf);
		cst_exit("Failed to create \"objs/internal\" directory", 3);
	}
	free(buf);
	cst_compile_internal("cst_config");
	if (CST_ARGS.memcheck)
		cst_compile_internal("internal/cst_memcheck");
	if (CST_ARGS.sighandler)
		cst_compile_internal("internal/cst_signal_handler");
	cst_vdebug(CST_BBLUE"Internals compiled in"CST_GRAY": "CST_BYELLOW"%zums", cst_now_ms() - compile_ms);
}

/*
 - Args validation
 */

static void cst_validate_args(void)
{
	int	e = 0;

	if (CST_ARGS.test_objs == NULL)
		e += printf(CST_ERR_PREFIX"No test objects provided"CST_RES"\n");
	if (CST_ARGS.proj_objs == NULL)
		e += printf(CST_ERR_PREFIX"No program objects provided"CST_RES"\n");
	if (e != 0)
		cst_exit(NULL, ARG_VALIDATION_ERRC);
}

/* 
 - Building args
 */

static char *cst_sanitize_arg(const char *arg)
{
	size_t	final_size = 0;
	size_t	si = 0;
	char	*sanitized;
	size_t	arglen = 0;

	for (; arg[arglen] != '\0'; arglen++)
		if (!cst_isspace(arg[arglen]) || (final_size != 0 && !cst_isspace(arg[arglen - 1])))
			final_size++;
	if (final_size == 0)
		return NULL;
	if (cst_isspace(arg[arglen - 1]))
		final_size--;
	sanitized = cst_malloc((final_size + 1) * sizeof(char));
	for (size_t i = 0; arg[i] != '\0' && si < final_size; i++) {
		char argc = arg[i];
		if (!cst_isspace(argc) || (si != 0 && !cst_isspace(arg[i - 1])))
			sanitized[si++] = isspace(argc) ? ' ' : argc;
	}
	sanitized[si] = '\0';
	return (sanitized);
}

static void cst_init_args(int argc, char **argv)
{
	CST_ARGS.test_objs = NULL;
	CST_ARGS.proj_objs = NULL;
	CST_ARGS.extra_flags = NULL;
	CST_ARGS.memcheck = true;
	CST_ARGS.sighandler = true;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (strncmp(arg, "proj_objs=", 10) == 0)
			CST_ARGS.proj_objs = cst_sanitize_arg(argv[i] + 10);
		else if (strncmp(arg, "test_objs=", 10) == 0)
			CST_ARGS.test_objs = cst_sanitize_arg(argv[i] + 10);
		else if (strncmp(arg, "cflags=", 7) == 0)
			CST_ARGS.extra_flags = cst_sanitize_arg(argv[i] + 7);
		else if (strncmp(arg, "cst_dir=", 8) == 0)
			CST_DIR = strdup(argv[i] + 8); // We trust the user on this one
		else if (strcmp(arg, "-debug") == 0 || strcmp(arg, "-d") == 0)
			CST_DEBUG = true;
		else if (strcmp(arg, "-nomem") == 0 || strcmp(arg, "-nomemcheck") == 0)
			CST_ARGS.memcheck = false;
		else if (strcmp(arg, "-nosig") == 0 || strcmp(arg, "-nosighandler") == 0)
			CST_ARGS.sighandler = false;
	}
}

/*
 - Get CST directory
 */

static char *cst_get_dir(const char *argv0)
{
	char	*dir = NULL;

	dir = realpath("/proc/self/exe", NULL);
	if (dir != NULL)
		return dirname(dir);
	if (argv0 != NULL && strchr(argv0, '/') != NULL) {
		dir = realpath(argv0, NULL);
		if (dir != NULL)
			return dirname(dir);
	}
	return NULL;
}

/*
 - Program entry point
 */

int main(int argc, char **argv)
{
	CST_START_DATE = cst_now_ms();
	cst_init_args(argc, argv);
	if (CST_DIR == NULL)
		CST_DIR = cst_get_dir(argv[0]);
	if (CST_DIR == NULL)
		cst_exit(CST_DIR_UNKNOWN_ERR, CST_DIR_UNKNOWN_ERRC);
	cst_vdebug(CST_BBLUE"CST Directory"CST_GRAY": "CST_YELLOW"%s", CST_DIR);
	cst_validate_args();
	cst_vdebug(CST_BBLUE"Internal signal handler"CST_GRAY": %s", (CST_ARGS.sighandler ? CST_GREEN"YES" : CST_RED"NO"));
	cst_vdebug(CST_BBLUE"Internal memcheck"CST_GRAY": %s", (CST_ARGS.memcheck ? CST_GREEN"YES" : CST_RED"NO"));
	cst_compile_internals();
	cst_tokenize_tests();
	cst_run_tests();
	cst_exit(NULL, EXIT_SUCCESS);
}
