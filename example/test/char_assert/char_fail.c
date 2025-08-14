
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "cst_toupper('a') == 'B'"
#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_FAIL_TIP = "Test failed intentionally";
	ASSERT_CHAR_EQUALS(cst_toupper('a'), 'B');
}
