
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "\"Hi\" == NULL"
#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	ASSERT_NULL("Hi");
}
