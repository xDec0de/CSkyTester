
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "sky_isnum('3')"
#include "cst_assertions.h"

#include <stdbool.h>

bool	sky_isnum(char ch);

int	main(void)
{
	ASSERT_TRUE(sky_isnum('3'));
}
