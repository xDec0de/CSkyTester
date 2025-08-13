#ifndef SKY_ASSERTIONS_H
# define SKY_ASSERTIONS_H

/* stdout & fprintf */
#include <stdio.h>

/* exit */
#include <stdlib.h>

/*
 - Assertions - Bool
 */

#define ASSERT_TRUE(expr) do {\
	if (!(expr)) {\
		fprintf(stderr, "Got FALSE when expecting TRUE from %s", #expr);\
		exit(EXIT_FAILURE);\
	}\
} while (0)

#define ASSERT_FALSE(expr) do {\
	if ((expr)) {\
		fprintf(stderr, "Got TRUE when expecting FALSE from %s", #expr);\
		exit(EXIT_FAILURE);\
	}\
} while (0)

#endif
