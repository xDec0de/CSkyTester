
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
static cst_args		ARGS;

static cst_category	*CST_CATEGORIES = NULL;

/*
 - Message utility
 */

static void	debug(char *msg)
{
	if (CST_DEBUG)
		printf(CST_GRAY"["CST_BWHITE"CST DEBUG"CST_GRAY"] "CST_WHITE"%s"CST_RES"\n", msg);
}

static void vdebug(const char *msg, ...)
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
	if (ARGS.test_objs != NULL)
		free(ARGS.test_objs);
	if (ARGS.proj_objs != NULL)
		free(ARGS.proj_objs);
	if (ARGS.extra_flags != NULL)
		free(ARGS.extra_flags);
	if (CST_DIR != NULL)
		free(CST_DIR);
	if (errmsg != NULL)
		printf(CST_ERR_PREFIX"%s"CST_RES"\n", errmsg);
	vdebug(CST_BBLUE"Time elapsed"CST_GRAY": "CST_BYELLOW"%zums", cst_now_ms() - CST_START_DATE);
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
 - Prepare test categories
 */

static void cst_prepare_test(size_t from, size_t to)
{
	const size_t	size = to - from;
	size_t			sep;
	char			*dir;
	char			*obj;

	obj = cst_malloc((size + 1) * sizeof(char));
	for (size_t i = from; i <= to; i++)
		obj[i - from] = ARGS.test_objs[i];
	obj[size] = '\0';
	vdebug("Obj: %s", obj);
	for (sep = to; sep != from && ARGS.test_objs[sep] != '/'; sep--)
		;
	if (sep == from)
		dir = NULL;
	else {
		dir = cst_malloc((sep - from + 1) * sizeof(char));
		for (size_t i = from; i <= sep; i++)
			dir[i - from] = ARGS.test_objs[i];
		dir[sep - from] = '\0';
	}
	vdebug("- Dir: %s", dir);
	if (dir != NULL)
		free(dir);
	free(obj);
}

static void cst_prepare_categories(void)
{
	size_t	i = 0;
	size_t	last_i = 0;
	char	ch;

	for (; (ch = ARGS.test_objs[i]) != '\0'; i++) {
		if (ch == ' ') {
			cst_prepare_test(last_i, i);
			last_i = i + 1;
		}
	}
	cst_prepare_test(last_i, i);
	free(ARGS.test_objs);
	ARGS.test_objs = NULL;
}

/* 
 - Internal sources compilation
 */

static bool is_up_to_date(const char *src, const char *obj)
{
	struct stat st_src, st_obj;

	if (stat(obj, &st_obj) == -1)
		return false;
	if (stat(src, &st_src) == -1)
		return false;
	return st_obj.st_mtime >= st_src.st_mtime;
}

static void compile_internal(const char *file)
{
	char *src = cst_fmt("%s/%s.c", CST_DIR, file);
	char *obj = cst_fmt("%s/objs/%s.o", CST_DIR, file);

	if (is_up_to_date(src, obj)) {
		vdebug(CST_BBLUE "Up to date" CST_GRAY ": " CST_YELLOW "%s", obj);
		free(src); free(obj);
		return;
	}
	char *cmd = cst_fmt("gcc -c %s -o %s", src, obj);
	vdebug(CST_BBLUE "Compiling" CST_GRAY ": " CST_YELLOW "%s", src);
	if (system(cmd) != 0) {
		free(src); free(obj); free(cmd);
		cst_exit(CST_COMPILE_INTERNAL_ERR, CST_COMPILE_INTERNAL_ERRC);
	}
	free(src); free(obj); free(cmd);
}

static void compile_internals(void)
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
	compile_internal("cst_config");
	if (ARGS.memcheck)
		compile_internal("internal/cst_memcheck");
	if (ARGS.sighandler)
		compile_internal("internal/cst_signal_handler");
	vdebug(CST_BBLUE"Internals compiled in"CST_GRAY": "CST_BYELLOW"%zums", cst_now_ms() - compile_ms);
}

/*
 - Args validation
 */

static bool validate_cst_args(void)
{
	int	e = 0;

	if (ARGS.test_objs == NULL)
		e += printf(CST_ERR_PREFIX"No test objects provided"CST_RES"\n");
	if (ARGS.proj_objs == NULL)
		e += printf(CST_ERR_PREFIX"No program objects provided"CST_RES"\n");
	return (e == 0);
}

/* 
 - Building args
 */

static char *sanitize_arg(const char *arg)
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

static void init_cst_args(int argc, char **argv)
{
	ARGS.test_objs = NULL;
	ARGS.proj_objs = NULL;
	ARGS.extra_flags = NULL;
	ARGS.memcheck = true;
	ARGS.sighandler = true;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (strncmp(arg, "proj_objs=", 10) == 0)
			ARGS.proj_objs = sanitize_arg(argv[i] + 10);
		else if (strncmp(arg, "test_objs=", 10) == 0)
			ARGS.test_objs = sanitize_arg(argv[i] + 10);
		else if (strncmp(arg, "cflags=", 7) == 0)
			ARGS.extra_flags = sanitize_arg(argv[i] + 7);
		else if (strncmp(arg, "cst_dir=", 8) == 0)
			CST_DIR = strdup(argv[i] + 8); // We trust the user on this one
		else if (strcmp(arg, "-debug") == 0 || strcmp(arg, "-d") == 0)
			CST_DEBUG = true;
		else if (strcmp(arg, "-nomem") == 0 || strcmp(arg, "-nomemcheck") == 0)
			ARGS.memcheck = false;
		else if (strcmp(arg, "-nosig") == 0 || strcmp(arg, "-nosighandler") == 0)
			ARGS.sighandler = false;
	}
}

/*
 - Get CST directory
 */

static char *get_cst_dir(const char *argv0)
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
	init_cst_args(argc, argv);
	if (CST_DIR == NULL)
		CST_DIR = get_cst_dir(argv[0]);
	if (CST_DIR == NULL)
		cst_exit(CST_DIR_UNKNOWN_ERR, CST_DIR_UNKNOWN_ERRC);
	vdebug(CST_BBLUE"CST Directory"CST_GRAY": "CST_YELLOW"%s", CST_DIR);
	if (!validate_cst_args())
		cst_exit(NULL, ARG_VALIDATION_ERRC);
	vdebug(CST_BBLUE"Test sources"CST_GRAY": "CST_YELLOW"\"%s\"", ARGS.test_objs);
	vdebug(CST_BBLUE"Proj sources"CST_GRAY": "CST_YELLOW"\"%s\"", ARGS.proj_objs);
	vdebug(CST_BBLUE"Extra flags"CST_GRAY": "CST_YELLOW"\"%s\"", ARGS.extra_flags);
	vdebug(CST_BBLUE"Internal signal handler"CST_GRAY": %s", (ARGS.sighandler ? CST_GREEN"YES" : CST_RED"NO"));
	vdebug(CST_BBLUE"Internal memcheck"CST_GRAY": %s", (ARGS.memcheck ? CST_GREEN"YES" : CST_RED"NO"));
	compile_internals();
	cst_prepare_categories();
	cst_exit(NULL, EXIT_SUCCESS);
}
