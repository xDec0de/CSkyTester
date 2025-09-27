#include "cst_assertions.h"

TEST("Memory tests", "Force memory leak") {
	char *ptr = malloc(42);
}
