
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "No category test"
#include "cst_assertions.h"

#include <stdbool.h>

bool	cst_isnum(char ch);

int	main(void)
{
	CST_FAIL_TIP = "Test failed intentionally";
	ASSERT_FALSE(cst_isnum('1'));
}
