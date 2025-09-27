#ifndef CST_INTERNALS_H
# define CST_INTERNALS_H

#include <stdbool.h>
#include <ctype.h>

typedef struct cst_test
{
	char			*category;
	char			*name;
	void			(*func)(void);
	bool			executed;
	struct cst_test	*next;
}	cst_test;

typedef struct cst_args
{
	bool	memcheck;
	bool	sighandler;
}	cst_args;

static inline int cst_isspace(int ch)
{
	return isspace((unsigned char) ch);
}

# define CST_ERR_PREFIX CST_RED"CST Error"CST_GRAY": "CST_BRED

# define ARG_VALIDATION_ERRC 1

# define CST_DIR_UNKNOWN_ERR "Couldn't obtain CST's directory. Try defining it manually with \"cst_dir=/absolute/path/to/cst\""
# define CST_DIR_UNKNOWN_ERRC 2

# define CST_COMPILE_INTERNAL_ERR "Failed to compile internal files"
# define CST_COMPILE_INTERNAL_ERRC 4

# define ALLOC_FAIL_ERR "Memory allocation failed"
# define ALLOC_FAIL_ERRC 100

#endif
