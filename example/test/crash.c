#include "cst_assertions.h"
#include "cst_example.h"


int	main(void)
{
	CST_TEST_NAME = "Force crash test";
	char *null_str = NULL;
	ASSERT_TRUE(null_str[0]);
}
