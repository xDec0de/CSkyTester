
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "Example test name"
#include "cst_assertions.h"

#include <stdbool.h>

bool	sky_isnum(char ch);

int	main(void)
{
	CST_FAIL_TIP = "Test failed intentionally to test format";
	ASSERT_FALSE(sky_isnum('1'));
}
