#ifndef CST_INTERNALS_H
# define CST_INTERNALS_H

#include <stdbool.h>
#include <ctype.h>

typedef struct cst_category
{
	char	*dir;
	char	*alias;
	char	**objs;
}	cst_category;

typedef struct cst_args
{
	char	*test_objs;
	char	*proj_objs;
	char	*extra_flags;
	bool	memcheck;
	bool	sighandler;
}	cst_args;

static inline int cst_isspace(int ch)
{
	return isspace((unsigned char) ch);
}

# define ARG_VALIDATION_ERRC 1
# define CST_DIR_UNKNOWN 2
# define ALLOC_FAIL_ERRC 100

#endif
