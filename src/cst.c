
#include "internal/cst_internals.h"
#include "cst_color.h"

static bool			CST_DEBUG = false;

static const char	*CST_SOURCES[] = {
	"cst_config.c",
	"internal/cst_memcheck.c",
	"internal/cst_signal_handler.c",
};

#define CST_SOURCES_COUNT (sizeof(CST_SOURCES) / sizeof(CST_SOURCES[0]))

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
		printf(CST_GRAY"["CST_BWHITE"CST DEBUG"CST_GRAY"] %s"CST_RES"\n", msg);
}

static void vdebug(const char *msg, ...)
{
	va_list	args;

	if (!CST_DEBUG)
		return ;
	va_start(args, msg);
	printf(CST_GRAY "[" CST_BWHITE "CST DEBUG" CST_GRAY "] ");
	vprintf(msg, args);
	va_end(args);
	printf(CST_RES "\n");
}

/*
 - Alloc valication
 */

static void	*validate_alloc(void *alloc)
{
	if (alloc != NULL)
		return alloc;
	err("Memory allocation failed");
	exit(ALLOC_FAIL_ERRC);
}

/*
 - Args validation
 */

static bool validate_cst_args(cst_args args)
{
	int e = 0;

	if (args.test_srcs == NULL)
		e += err("No test sources provided");
	if (args.proj_srcs == NULL)
		e += err("No program sources provided");
	if (e != 0)
		return (false);
	debug(CST_GREEN"Valid arguments provided"CST_GRAY":");
	vdebug(CST_BBLUE"  Test sources"CST_GRAY": "CST_YELLOW"\"%s\"", args.test_srcs);
	vdebug(CST_BBLUE"  Proj sources"CST_GRAY": "CST_YELLOW"\"%s\"", args.proj_srcs);
	vdebug(CST_BBLUE"  Extra flags"CST_GRAY": "CST_YELLOW"\"%s\"", args.extra_flags);
	return (true);
}

/* 
 - Building args
 */

static char *sanitize_arg(char *arg)
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
	sanitized = validate_alloc(malloc((final_size + 1) * sizeof(char)));
	for (size_t i = 0; arg[i] != '\0' && si < final_size; i++) {
		char argc = arg[i];
		if (!cst_isspace(argc) || (si != 0 && !cst_isspace(arg[i - 1])))
			sanitized[si++] = isspace(argc) ? ' ' : argc;
	}
	sanitized[si] = '\0';
	return (sanitized);
}

static cst_args init_cst_args(int argc, char **argv)
{
	cst_args args;

	args.test_srcs = NULL;
	args.proj_srcs = NULL;
	args.extra_flags = NULL;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (strncmp(arg, "proj_srcs=", 10) == 0)
			args.proj_srcs = sanitize_arg(argv[i] + 10);
		else if (strncmp(arg, "test_srcs=", 10) == 0)
			args.test_srcs = sanitize_arg(argv[i] + 10);
		else if (strcmp(arg, "-debug") == 0 || strcmp(arg, "-d") == 0)
			CST_DEBUG = true;
	}
	return (args);
}

/*
 - Program entry / exit points
 */

static void	cst_exit(cst_args args, int ec)
{
	if (args.test_srcs != NULL)
		free(args.test_srcs);
	if (args.proj_srcs != NULL)
		free(args.proj_srcs);
	if (args.extra_flags != NULL)
		free(args.extra_flags);
	exit(ec);
}

int main(int argc, char **argv)
{
	cst_args args = init_cst_args(argc, argv);

	if (!validate_cst_args(args))
		cst_exit(args, ARG_VALIDATION_ERRC);
	cst_exit(args, EXIT_SUCCESS);
}
