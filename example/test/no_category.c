
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "No category test"
#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_SHOW_FAIL_DETAILS = false;
	CST_FAIL_TIP = "Custom assertion fail details (Tests fail intentionally)";
	ASSERT_FALSE(cst_isnum('1'));
}
