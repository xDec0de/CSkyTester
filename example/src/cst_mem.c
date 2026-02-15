#include <stdlib.h>

void	cst_force_leak(void)
{
	void *leak = malloc(42);
	(void) leak;
}

void	cst_force_double_free(void)
{
	void *ptr = malloc(42);
	free(ptr);
	free(ptr);
}
