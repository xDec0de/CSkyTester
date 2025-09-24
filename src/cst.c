
#include "internal/cst_internals.h"
#include "cst_color.h"
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>

static char			*CST_DIR = NULL;
static cst_args		ARGS;
static bool			CST_DEBUG = false;

static const char	*CST_SOURCES =
	"cst_config.c \
	internal/cst_memcheck.c \
	internal/cst_signal_handler.c";

/*
 - Message utility
 */

static int	err(char *msg)
{
	return printf(CST_RED"CST Error"CST_GRAY": "CST_BRED"%s"CST_RES"\n", msg);
}

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
	if (err != NULL)
		err(errmsg);
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
 - Args validation
 */

static bool validate_cst_args(void)
{
	int	e = 0;

	if (ARGS.test_objs == NULL)
		e += err("No test sources provided");
	if (ARGS.proj_objs == NULL)
		e += err("No program sources provided");
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
	init_cst_args(argc, argv);
	if (CST_DIR == NULL)
	CST_DIR = get_cst_dir(argv[0]);
	if (CST_DIR == NULL) {
		cst_exit(CST_DIR_UNKNOWN_ERR, CST_DIR_UNKNOWN_ERRC);
	vdebug(CST_BBLUE"CST Directory"CST_GRAY": "CST_YELLOW"%s", CST_DIR);
	if (!validate_cst_args())
		cst_exit(NULL, ARG_VALIDATION_ERRC);
	vdebug(CST_BBLUE"Test sources"CST_GRAY": "CST_YELLOW"\"%s\"", ARGS.test_objs);
	vdebug(CST_BBLUE"Proj sources"CST_GRAY": "CST_YELLOW"\"%s\"", ARGS.proj_objs);
	vdebug(CST_BBLUE"Extra flags"CST_GRAY": "CST_YELLOW"\"%s\"", ARGS.extra_flags);
	vdebug(CST_BBLUE"Internal signal handler"CST_GRAY": %s", (ARGS.sighandler ? CST_GREEN"YES" : CST_RED"NO"));
	vdebug(CST_BBLUE"Internal memcheck"CST_GRAY": %s", (ARGS.memcheck ? CST_GREEN"YES" : CST_RED"NO"));
	cst_exit(NULL, EXIT_SUCCESS);
}
