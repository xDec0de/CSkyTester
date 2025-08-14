
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "cst_isnum('1')"
#include "cst_assertions.h"

#include <stdbool.h>

bool	cst_isnum(char ch);

int	main(void)
{
	ASSERT_TRUE(cst_isnum('1'));
}
