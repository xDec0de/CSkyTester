
// The definition of CST_TEST_NAME MUST be BEFORE including cst_assertions.h

#define CST_TEST_NAME "cst_utoupper('a') == 'A'"
#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	ASSERT_CHAR_EQUALS(cst_utoupper((unsigned char) 'a'), (unsigned char) 'A');
}
