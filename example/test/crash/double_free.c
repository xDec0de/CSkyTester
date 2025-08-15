#include "cst_assertions.h"

int	main(void)
{
	char	*ptr = malloc(sizeof(char *));

	CST_TEST_NAME = "Double free";
	free(ptr);
	free(ptr);
}