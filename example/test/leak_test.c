#include "cst_assertions.h"

TEST("Memory tests", "Force memory leak (Shouldn't pass)") {
	char *ptr = malloc(42);
	ASSERT_NULL(NULL);
}
