
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "\"Hi\" == NOT NULL"
#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	ASSERT_NOT_NULL("Hi");
}
