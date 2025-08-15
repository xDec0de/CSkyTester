#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "cst_toupper('a') == 'B'";
	ASSERT_CHAR_EQUALS(cst_toupper('a'), 'B');
}
