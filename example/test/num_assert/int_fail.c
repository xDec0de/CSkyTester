#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "cst_intsum(40, 2) == 24";
	ASSERT_INT_EQUALS(cst_intsum(40, 2), 24);
}
