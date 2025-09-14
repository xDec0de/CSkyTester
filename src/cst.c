
#include "internal/cst_internals.h"
#include "cst_color.h"

static const char *CST_SOURCES[] = {
	"cst_config.c",
	"internal/cst_memcheck.c",
	"internal/cst_signal_handler.c",
};

#define CST_SOURCES_COUNT (sizeof(CST_SOURCES) / sizeof(CST_SOURCES[0]))

/*
 - Message utility
 */

int	err(char *msg)
{
	return printf(CST_RED"Error"CST_GRAY": "CST_BRED"%s"CST_RES"\n", msg);
}

/*
 - Program args handling
 */

bool validate_cst_args(cst_args args)
{
	int e = 0;

	if (args.test_srcs == NULL)
		e += err("No test sources provided");
	if (args.proj_srcs == NULL)
		e += err("No program sources provided");
	return (e == 0);
}

cst_args init_cst_args(int argc, char **argv)
{
	cst_args args;

	args.test_srcs = NULL;
	args.proj_srcs = NULL;
	args.extra_flags = NULL;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (strncmp(arg, "proj_srcs=", 10) == 0)
			args.proj_srcs = strdup(argv[i] + 10);
		else if (strncmp(arg, "test_srcs=", 10) == 0)
			args.test_srcs = strdup(argv[i] + 10);
	}
	return (args);
}

/*
 - Program entry point
 */

int main(int argc, char **argv)
{
	cst_args args = init_cst_args(argc, argv);

	if (!validate_cst_args(args))
		exit(1);
	printf(CST_GREEN"Valid arguments provided"CST_GRAY":"CST_RES"\n");
	printf(CST_BBLUE"  Test sources"CST_GRAY": "CST_YELLOW"%s"CST_RES"\n", args.test_srcs);
	printf(CST_BBLUE"  Proj sources"CST_GRAY": "CST_YELLOW"%s"CST_RES"\n", args.proj_srcs);
	printf(CST_BBLUE"  Extra flags"CST_GRAY": "CST_YELLOW"%s"CST_RES"\n", args.extra_flags);
}
