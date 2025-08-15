#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "cst_isnum('a') == true";
	ASSERT_TRUE(cst_isnum('a'));
}
