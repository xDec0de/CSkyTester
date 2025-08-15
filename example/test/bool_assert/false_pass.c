#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "cst_isnum('1') == true";
	ASSERT_TRUE(cst_isnum('1'));
}
