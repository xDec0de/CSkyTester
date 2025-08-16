#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	char *ptr = malloc(42);
	CST_TEST_NAME = "Force memory leak";
}
