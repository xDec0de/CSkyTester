#ifndef CST_INTERNALS_H
# define CST_INTERNALS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

typedef struct cst_category
{
	char	*dir;
	char	*alias;
	char	**files;
}	cst_category;

typedef struct cst_args
{
	char	*test_objs;
	char	*proj_objs;
	char	*extra_flags;
}	cst_args;

static inline int cst_isspace(int ch)
{
	return isspace((unsigned char) ch);
}

# define ARG_VALIDATION_ERRC 1
# define ALLOC_FAIL_ERRC 100

#endif
