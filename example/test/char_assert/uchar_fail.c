#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "cst_utoupper('a') == 'B'";
	ASSERT_CHAR_EQUALS(cst_utoupper((unsigned char) 'a'), (unsigned char) 'B');
}
