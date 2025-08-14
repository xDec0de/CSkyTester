
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "cst_uintsum(40, 2) == 42"
#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	ASSERT_UINT_EQUALS(cst_uintsum(40, 2), 42);
}
