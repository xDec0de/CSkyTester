#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "cst_uintsum(40, 2) == 42";
	ASSERT_UINT_EQUALS(cst_uintsum(40, 2), 42);
}
