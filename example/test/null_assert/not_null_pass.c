#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "\"Hi\" == NOT NULL";
	ASSERT_NOT_NULL("Hi");
}
