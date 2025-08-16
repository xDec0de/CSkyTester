#include "cst_assertions.h"

int	main(void)
{
	char	*ptr = malloc(sizeof(char *));

	CST_TEST_NAME = "Force double free";
	free(ptr);
	free(ptr);
}